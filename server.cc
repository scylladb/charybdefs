/*
 * ** 27-12-2015
 * **
 * ** The author disclaims copyright to this source code.  In place of
 * ** a legal notice, here is a blessing:
 * **
 * **    May you do good and not evil.
 * **    May you find forgiveness for yourself and forgive others.
 * **    May you share freely, never taking more than you give.
 * **
 */

#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <random>
#include <set>
#include <regex>
#include <thread>
#include <vector>

#include <cerrno>
#include <sys/types.h>
#include <csignal>

#include <fuse.h>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

#include "gen-cpp/server.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using std::shared_ptr;

struct fault_descriptor {
    bool random;                                // error code must be randomized
    std::vector<int32_t>  err_nos;              // error code(s) to select from
    int32_t probability;                        // 0 < probability < 100
    std::string regexp;                         // regular expression on filename
    bool kill_caller;                           // Must we kill the caller
    int32_t delay_us;                           // operation delay in us
    bool auto_delay;                            // must auto delay like an SSD
};

std::vector<int32_t> default_errnos;
std::set<std::string> valid_methods;
std::map<std::string, fault_descriptor> fault_map;
std::mutex mutex;

void init_default_errnos()
{
    int32_t errno;
    for (errno = E2BIG; errno < EXFULL; errno++) {
        default_errnos.push_back(errno);
    }
}

void init_valid_methods()
{
    valid_methods.insert("getattr");
    valid_methods.insert("readlink");
    valid_methods.insert("mknod");
    valid_methods.insert("mkdir");
    valid_methods.insert("unlink");
    valid_methods.insert("rmdir");
    valid_methods.insert("symlink");
    valid_methods.insert("rename");
    valid_methods.insert("link");
    valid_methods.insert("chmod");
    valid_methods.insert("chown");
    valid_methods.insert("truncate");
    valid_methods.insert("open");
    valid_methods.insert("read");
    valid_methods.insert("write");
    valid_methods.insert("statfs");
    valid_methods.insert("flush");
    valid_methods.insert("release");
    valid_methods.insert("fsync");
    valid_methods.insert("setxattr");
    valid_methods.insert("getxattr");
    valid_methods.insert("listxattr");
    valid_methods.insert("removexattr");
    valid_methods.insert("opendir");
    valid_methods.insert("readdir");
    valid_methods.insert("releasedir");
    valid_methods.insert("fsyncdir");
    valid_methods.insert("access");
    valid_methods.insert("create");
    valid_methods.insert("ftruncate");
    valid_methods.insert("fgetattr");
    valid_methods.insert("lock");
    valid_methods.insert("bmap");
    valid_methods.insert("ioctl");
    valid_methods.insert("poll");
    valid_methods.insert("write_buf");
    valid_methods.insert("read_buf");
    valid_methods.insert("flock");
    valid_methods.insert("fallocate");
}

static bool is_valid_method(std::string method)
{
    return valid_methods.count(method);
}

// return a random err_no
static int random_err_no(std::vector<int32_t> err_nos)
{
    if (err_nos.empty() == 0) {
        err_nos = default_errnos;
    }

    std::random_device rd;
    std::uniform_int_distribution<int> dist(0, err_nos.size());
    return int(err_nos[dist(rd)]);
}

// return true if random number is not in the probability
static bool get_lucky(int probability)
{
    std::random_device rd;
    std::uniform_int_distribution<int> dist(1, 100000);

    if (!probability) {
        return false;
    }

    if (dist(rd) > probability) {
        return true;
    }

    return false;
}

// return an err_no if we must proceed to error injection
int error_inject(volatile int in_flight, std::string path, std::string method)
{
    std::lock_guard<std::mutex> lk(mutex);

    // no fault injection for this method
    if (!fault_map.count(method)) {
        return 0;
    }

    // get the fault injection descriptor
    auto descr = fault_map[method];

    // get the err_no to inject
    int err_no = 0;
    if (descr.random || !descr.err_nos.empty()) {
        err_no = random_err_no(descr.err_nos);
    }

    if (descr.regexp.size()) {
        std::regex r(descr.regexp);
        if (!std::regex_match(path, r)) {
            return 0;
        }
    }

    // Are we out of error probability
    if (get_lucky(descr.probability)) {
        return 0;
    }

    uint32_t delay = 0;
    if (descr.delay_us) {
        delay = descr.delay_us;
    }

    if (descr.auto_delay) {
        delay = 0;
    }

    if (delay) {
        std::this_thread::sleep_for(
            std::chrono::microseconds(delay));
    }

    if (descr.kill_caller) {
        static struct fuse_context *context;
        context = fuse_get_context();
        kill(context->pid, SIGKILL);
        return 0;
    }

    return -err_no;
}

class server_handler: public serverIf {
    void get_methods(std::vector<std::string> & _return)
    {
        for (auto method: valid_methods) {
            _return.push_back(method);
        }
    }

    void clear_all_faults()
    {
        std::lock_guard<std::mutex> lk(mutex);
        fault_map.clear();
    }

    void clear_fault(const std::string& method)
    {
        std::lock_guard<std::mutex> lk(mutex);
        fault_map.erase(method);
    }

    void set_fault(const std::vector<std::string>& methods, const bool random,
                   const std::vector<int32_t> & err_nos, const int32_t probability,
                   const std::string& regexp, const bool kill_caller,
                   int32_t delay_us, const bool auto_delay)
    {
        struct fault_descriptor descr;

        descr.random = random;
        descr.err_nos = err_nos;
        descr.probability = probability;
        descr.regexp = regexp;
        descr.kill_caller = kill_caller;
        descr.delay_us = delay_us;
        descr.auto_delay = auto_delay;

        std::lock_guard<std::mutex> lk(mutex);
        for (auto method: methods) {
            if (is_valid_method(method)) {
                fault_map[method] = descr;
            }
        }
    }

    void set_all_fault(const bool random, const std::vector<int32_t> & err_nos,
                       const int32_t probability, const std::string& regexp,
                       const bool kill_caller, const int32_t delay_us,
                       const bool auto_delay)
    {
        std::vector<std::string> methods;

        for (auto method: valid_methods) {
            methods.push_back(method);
        }

        set_fault(methods, random, err_nos, probability,
                  regexp, kill_caller, delay_us,
                  auto_delay);
    }

};

void server_thread()
{
    int port = 9090;

    init_valid_methods();
    init_default_errnos();

    std::cout << "Server Thread started" << std::endl;
    try {
        shared_ptr<server_handler> handler(new server_handler());
        shared_ptr<TProcessor> processor(new serverProcessor(handler));
        shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
        shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
        shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

        TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
        std::cout << "Starting to serve" << std::endl;
        server.serve();
    } catch(...) {
        std::cout << "Caught exception" << std::endl;
    }
    std::cout << "Finished serving" << std::endl;
}

std::thread *thread;

void start_server_thread() {
    thread = new std::thread(server_thread);
}
