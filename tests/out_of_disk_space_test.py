#!/usr/bin/python
import random

from common import *


def stress():
    FNULL = open(os.devnull, 'w')
    proc = subprocess.Popen(['/usr/bin/cassandra-stress',
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


def cleanup(scylla_proc, scylla_log, stress_proc):
    print("Stopping cassandra stress")
    os.killpg(os.getpgid(stress_proc.pid), signal.SIGTERM)
    stress_proc.wait()

    stop_scylla(scylla_proc, scylla_log)


def main():
    client = connect()
    client.clear_all_faults()

    scylla_proc, scylla_log = start_scylla("out-of-disk-space", True)

    print("Starting cassandra stress")
    stress_proc = stress()
    # Sufficient time for activity such as memtable write, compaction, etc.
    time.sleep(random.randint(20, 50))

    methods = client.get_methods()
    ENOSPC = 28  # no space left on device error
    print("Creating out of disk space scenario")
    client.set_fault(methods, False, ENOSPC, 100000, "", False, 0, False)

    # Leave system with no space left for a few seconds.
    time.sleep(20)

    # Check that the system faced the  scenario
    if not has_message("out-of-disk-space", b'No space left on device'):
        print("Failed to make system run out of disk space")
        cleanup(scylla_proc, scylla_log, stress_proc)
        sys.exit(1)
    print("System is out of disk space as expected")

    # Clean faults and let the system recover from the out of disk space scenario.
    print("Destroying out of disk space scenario")
    client.clear_all_faults()
    scylla_log.seek(0)
    scylla_log.truncate()
    time.sleep(20)

    # Checking that the system was actually able to recover
    if has_message("out-of-disk-space", b'No space left on device'):
        print("Failed to make system recover from out of disk space scenario")
        cleanup(scylla_proc, scylla_log, stress_proc)
        sys.exit(1)
    print("System is no longer out of disk space as expected")

    cleanup(scylla_proc, scylla_log, stress_proc)

    print("Success")
    print("")

if __name__ == "__main__":
    main()
