import errno

import sys

sys.path.append('gen-py')
from server import server
from server.ttypes import *

from thrift.transport import TSocket
from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol


def usage():
    print("Choose one of the following option:\n"
          " --clear\n"
          " --full\n"
          " --io-error\n"
          " --quota\n"
          " --delay\n"
          " --random\n"
          " --specific-syscalls\n"
          " --probability\n"
          " --file-pattern\n"
          " --broken-drive")
    sys.exit(1)


def connect():
    transport = TSocket.TSocket('127.0.0.1', 9090)
    transport = TTransport.TBufferedTransport(transport)
    protocol = TBinaryProtocol.TBinaryProtocol(transport)
    client = server.Client(protocol)
    transport.open()
    return client


def main():
    if len(sys.argv) < 2:
        usage()

    client = connect()

    client.clear_all_faults()

    if sys.argv[1] == "--clear":
        print("Clearing all faults conditions")
        sys.exit(0)
    elif sys.argv[1] == "--full":
        print("Simulating disk full")
        client.set_all_fault(False, [errno.ENOSPC], 0, "", False, 0, False)
    elif sys.argv[1] == "--io-error":
        print("Simulating IO error")
        client.set_all_fault(False, [errno.EIO], 0, "", False, 0, False)
    elif sys.argv[1] == "--quota":
        print("Simulating quota exceeded")
        client.set_all_fault(False, [errno.EDQUOT], 0, "", False, 0, False)
    elif sys.argv[1] == "--delay":
        print("Simulating delayed IO")
        client.set_all_fault(False, [], 0, "", False, 50000, False)
    elif sys.argv[1] == "--random":
        # Use all errnos, minus any specified in trailing arguments.
        errnos_selected = {code: name for code, name in
                errno.errorcode.items() if name not in sys.argv[1:]}
        print("Simulating random errno")
        print("Using errnos: " + ", ".join(errnos_selected.values()))
        client.set_all_fault(True, errnos_selected.keys(), 0, "", False, 0, False)
    elif sys.argv[1] == "--specific-syscalls":
        print("Restricting random IO restricted to specific syscalls")
        client.set_fault(['read', 'read_buf', 'write', 'write_buf'], True, [], 0, "", False, 0, False)
    elif sys.argv[1] == "--probability":
        print("Restricting random IO restricted to specific syscalls and 1% error probability")
        client.set_fault(['read', 'read_buf', 'write', 'write_buf'], True, [], 1000, "", False, 0, False)
    elif sys.argv[1] == "--file-pattern":
        print("Restricting random IO restricted to specific syscalls while cursing *.sendmail.cf")
        client.set_fault(['read', 'read_buf', 'write', 'write_buf'], True, [], 0, ".*sendmail.cf", False, 0, False)
    elif sys.argv[1] == "--broken-drive":
        print("The agonising drive simulator")
        client.set_all_fault(False, [errno.EIO], 100, "", False, 100000, False)
    else:
        usage()


if __name__ == "__main__":
    main()
