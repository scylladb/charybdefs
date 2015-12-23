
CharibeFS
=========

A fuse based fault injection filesystem
with a Thrift RPC interface for instrumentation.

Building
========

cmake CMakeLists.txt
make

Using
=====

mkdir /tmp/scylla
sudo ./charibefs /var/lib/scylla/ -omodules=subdir,subdir=/tmp/scylla
