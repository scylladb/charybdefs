import os
import sys
import shutil
import time
import mmap
import signal
import subprocess

from thrift.transport import TSocket
from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol

sys.path.append('gen-py')
from server import server
from server.ttypes import *


def clear_scylla_dir():
    shutil.rmtree("/var/lib/scylla/commitlog", ignore_errors=True)
    shutil.rmtree("/var/lib/scylla/data", ignore_errors=True)


def build_log_filename(method, write):
    op = "read"
    if write:
        op = "write"
    return "scylla-%s-log-for-%s.txt" % (op, method)


def start_scylla(method, write):
    print("Starting scylla")
    log = open(build_log_filename(method, write), 'w')
    proc = subprocess.Popen(['/usr/bin/scylla',
                             '-m', '512M',
                             '--developer-mode=1'],
                            preexec_fn=os.setsid,
                            stdout=log, stderr=log)
    time.sleep(60)
    return proc, log


def stop_scylla(proc, log):
    os.killpg(os.getpgid(proc.pid), signal.SIGTERM)
    proc.wait()
    log.close()


def has_message(method, message):
    result = False
    f = open(build_log_filename(method, True))
    s = mmap.mmap(f.fileno(), 0, access=mmap.ACCESS_READ)
    if s.find(message) != -1:
        result = True
    s.close()
    f.close()
    return result


def connect():
    envPort = os.getenv('CHARYBDEFS_PORT', '')
    port = int(envPort) if envPort else 9090
    transport = TSocket.TSocket('127.0.0.1', port)
    transport = TTransport.TBufferedTransport(transport)
    protocol = TBinaryProtocol.TBinaryProtocol(transport)
    client = server.Client(protocol)
    transport.open()
    return client
