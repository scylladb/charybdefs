
CharybdeFS
==========

A fuse based fault injection filesystem
with a Thrift RPC interface for instrumentation.

Building
========

cmake CMakeLists.txt
make

Using
=====

```sh
mkdir /tmp/scylla
sudo ./charibefs /var/lib/scylla/ -omodules=subdir,subdir=/tmp/scylla
```

Example tests
=============

`tests/scylla_test_all_calls.py` is an example test running against the
ScyllaDB database. It will insert data in the database, trigger some
errors for a while, clear it and let the database recover.
After this it will boot up scylla again and check that all commited
queries are safe on disk.
