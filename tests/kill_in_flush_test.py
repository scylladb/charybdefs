import sys, glob

from subprocess import check_output
import subprocess
import signal
import random
import time
import sys
import os

from thrift import Thrift
from thrift.transport import TSocket
from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol

from common import *

def stress():
    FNULL = open(os.devnull, 'w')
    proc = subprocess.Popen(['/root/scylla-tools-java/tools/bin/cassandra-stress',
                             'write',
                             'duration=15m',
                             '-mode',
                             'native',
                             'cql3',
                             '-rate',
                             'threads=700',
                             '-node',
                             'localhost'],
                             preexec_fn=os.setsid,
                             stdout=FNULL,
                             stderr=subprocess.STDOUT)
    return proc

def is_running(name):
    try:
        check_output(["pidof",name]) == 0
    except subprocess.CalledProcessError, e:
        return False
    return True

def loop():
    client = connect()
    client.clear_all_faults()

    clear_scylla_dir()
    scylla_proc, scylla_log = start_scylla("fsync", True)    
    time.sleep(5)

    # start the stress and wait
    print "starting cassandra stress"
    stress_proc = stress()
    # setup a random time to hit every write path at random
    # commitlog / compaction / SSTable creation ...
    time.sleep(random.randint(15, 50))

    # trigger kill on flush/sync and wait it work
    print "setting flush/sync to kill scylla"
    client.set_fault(['flush', 'fsync', 'fsyncdir'], False, 0, 100000, "", True, 0, False)
    print "Waiting for scylla do die"
    while is_running("scylla"):
        time.sleep(1)
    print "Scylla died"
    print "clearing flush/sync kill"
    client.clear_all_faults()

    print "stopping cassandra stress"
    os.killpg(os.getpgid(stress_proc.pid), signal.SIGTERM)
    stress_proc.wait()

    scylla_proc, scylla_log = start_scylla("fsync-recover", True)

    time.sleep(60)

    if not is_running("scylla"):
        print "Scylla failed to boot after fsync kill. Something bad happened."
        sys.exit(1)

    stop_scylla(scylla_proc, scylla_log)

    if not has_message("fsync-recover", b'Starting listening for CQL clients'):
        print "Scylla did not boot completely after fsync kill. Something bad happened"
        sys.exit(2)

    print "Success"
    print ""

def main():
    while True:
        loop()

if __name__ == "__main__":
    main()
