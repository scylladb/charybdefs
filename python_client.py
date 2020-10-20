import sys
sys.path.append('gen-py')

from gen.server import server
from gen.server.ttypes import *

from thrift import Thrift
from thrift.transport import TSocket
from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol

try:

    transport = TSocket.TSocket('127.0.0.1', 9090)
    transport = TTransport.TBufferedTransport(transport)
    protocol = TBinaryProtocol.TBinaryProtocol(transport)
    client = server.Client(protocol)
    transport.open()

    print(client.get_methods())

    # client.set_fault(['flush', 'fsync', 'fsyncdir'], False, [], 100000, "", True, 500000)
    client.set_fault(['flush', 'fsync', 'fsyncdir'], False, [], 99000, "", True, 500000)
    # client.clear_all_faults()

except Thrift.TException as tx:
    print('%s' % tx.message)
