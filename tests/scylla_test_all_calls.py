#!/usr/bin/python
import random
import threading
import sys
import os
basedir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
gen_py_dir = os.path.join(basedir, 'cookbook', 'gen-py')
sys.path.append(gen_py_dir)
from server.ttypes import *
from common import *


def create_database(session):
    session.execute("""CREATE KEYSPACE sha WITH
                    replication = {'class': 'SimpleStrategy', 'replication_factor': 1}""")

    session.execute("""CREATE TABLE sha.test (
                       data ascii PRIMARY KEY,
                    );""")
    session.execute("""Use sha""")

counter = 0
keep_going = False
lock = threading.Lock()


def mkdata(gen):
    res = "".join([str(x) for x in (gen + x for x in range(1000))])
    gen += 1000
    return res, gen


def writer():
    """thread writer function"""
    from cassandra.cluster import Cluster
    cluster = Cluster()
    session = cluster.connect()

    create_database(session)

    global keep_going
    global counter
    global lock
    counter = 0
    gen = 0
    keep_going = True
    random.seed(0)
    lock.acquire()
    while keep_going:
        data, gen = mkdata(gen)
        try:
            query = "INSERT INTO sha.test (data) VALUES('%s');" % (data)
            session.execute(query)
        except Exception as e:
            print(str(e))
            keep_going = False
            continue
        counter += 1
        # let a change to the other thread to acquire the lock
        lock.release()
        lock.acquire()
    lock.release()


def read_check(count):
    global keep_going
    from cassandra.cluster import Cluster
    cluster = Cluster()
    session = cluster.connect()

    gen = 0
    counter = 0
    random.seed(0)

    while counter <= count:
        data, gen = mkdata(gen)

        try:
            query = "SELECT data FROM sha.test WHERE data='%s';" % data
            rows = session.execute(query)
            count = 0
            for row in rows:
                count += 1
            if count != 1:
                return False
        except Exception as e:
            print(str(e))
            keep_going = False
            continue

        counter += 1

    return True


def start_writing():
    t = threading.Thread(target=writer)
    t.start()
    return t


def stop_writing(t):
    global keep_going
    global lock

    lock.acquire()
    keep_going = False
    res = counter
    lock.release()

    t.join()

    return res


def test_method(method, client):
    print("Testing %s" % method)
    clear_scylla_dir()
    scylla_proc, scylla_log = start_scylla(method, True)

    write_thread = start_writing()
    import random
    time.sleep(random.randint(300, 1000))

    client.set_fault([method], True, 0, 100000, "", False, 0, False)
    time.sleep(5)
    client.clear_all_faults()

    if has_message(method, b'ERROR') and\
       not has_message(method, b'Shutdown communications until operator examinate the situation.'):
        print("Error handling method=%s" % method)
        sys.exit(1)

    time.sleep(30)

    count = stop_writing(write_thread)
    stop_scylla(scylla_proc, scylla_log)

    result = False
    scylla_proc, scylla_log = start_scylla(method, False)
    try:
        result = read_check(count)
    except Exception as e:
        print("Read check error for method=%s: %s" % (method, str(e)))
        sys.exit(2)
    if not result:
        print("Error in %s" % method)
        sys.exit(3)
    stop_scylla(scylla_proc, scylla_log)


def main():
    client = connect()

    methods = client.get_methods()

    while True:
        for method in methods:
            test_method(method, client)

if __name__ == "__main__":
    main()
