cmake_minimum_required (VERSION 2.8.11)
project (CharybdeFS)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "-std=c++11 -D_FILE_OFFSET_BITS=64")
set(CMAKE_C_FLAGS "-D_FILE_OFFSET_BITS=64")

link_directories(
        /usr/local/lib
        /usr/lib
)

find_package(FUSE 2.9 REQUIRED)

add_library(charybde_lib charybde_ops.cc server.cc)

add_custom_target(thrift
                  COMMAND thrift --gen cpp ${PROJECT_SOURCE_DIR}/server.thrift
                  BYPRODUCTS gen-cpp/server_constants.cpp gen-cpp/server.cpp gen-cpp/server_types.cpp)
add_dependencies(charybde_lib thrift)

add_library(server_lib charybde_ops.cc server.cc)

add_library(server gen-cpp/server_constants.cpp gen-cpp/server.cpp gen-cpp/server_types.cpp)

# find thrift headers and libs
find_package(Thrift REQUIRED)
include_directories(SYSTEM ${THRIFT_INCLUDE_DIR})

add_executable(charybdefs charybdefs.c)

target_link_libraries(charybdefs charybde_lib)
target_link_libraries(charybdefs server)
target_link_libraries(charybdefs ${FUSE_LIBRARIES})
target_link_libraries(charybdefs pthread)
target_link_libraries(charybdefs event)
target_link_libraries(charybdefs ${THRIFT_LIBS})
