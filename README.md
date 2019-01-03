
CharybdeFS
==========

A fuse based fault injection filesystem
with a Thrift RPC interface for instrumentation.

Building
========
**CentOS 7.4**

```sh
sudo yum install gcc-c++ cmake fuse fuse-devel thrift python-thrift thrift-devel
thrift -r --gen cpp --gen py server.thriftz
cmake CMakeLists.txt
make
```
**Ubuntu 16.04**

```sh
sudo apt-get install software-properties-common
sudo add-apt-repository ppa:scylladb/ppa
sudo apt-get update
sudo apt-get install g++ cmake fuse libfuse-dev python-thrift thrift-compiler scylla-libthrift010-dev libboost-dev
thrift -r --gen cpp --gen py server.thrift
cmake -DCMAKE_PREFIX_PATH=/opt/scylladb CMakeLists.txt
make
```

Using
=====
Load fuse module, if it is not loaded
```sh
modprobe fuse
```
Create mount directory for Charybde. Tested application should use this directory.

Note: Faults can be applied for files that are manipulated through this directory only!
```sh
mkdir /mnt/charybdbde
```
`dest_dir` directory on the file system where actual files will be stored
Running CharybdeFS
```sh
./charybdefs /mnt/charybdbde -omodules=subdir,subdir=/dest_dir
```
Running CharybdeFS and allowing other users to use the mount
```sh
./charybdefs /mnt/charybdbde -omodules=subdir,subdir=/dest_dir -oallow_other
```

Example tests
=============

`tests/scylla_test_all_calls.py` is an example test running against the
ScyllaDB database. It will insert data in the database, trigger some
errors for a while, clear it and let the database recover.
After this it will boot up scylla again and check that all commited
queries are safe on disk.
