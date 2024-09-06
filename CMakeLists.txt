cmake_minimum_required (VERSION 3.5.1)
project (Persephone C CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/scripts/cmake")

#include(dpdk)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(RELEASE_CFLAGS "-Wno-register -O3 -mtune=intel -DNDEBUG" CACHE STRING "C compiler options for release builds")
set(DEBUG_CFLAGS "-ggdb -O0 -fno-omit-frame-pointer -frecord-gcc-switches -lasan -fsanitize=address " CACHE STRING "C compiler options for debug builds")
set(CMAKE_C_FLAGS_DEBUG ${DEBUG_CFLAGS})
set(CMAKE_CXX_FLAGS_DEBUG ${DEBUG_CFLAGS})
set(CMAKE_C_FLAGS_RELEASE ${RELEASE_CFLAGS})
set(CMAKE_CXX_FLAGS_RELEASE ${RELEASE_CFLAGS})

set(CMAKE_VERBOSE_MAKEFILE ON)

set(FAKE_WORK_DIR ${CMAKE_SOURCE_DIR}/../afp/deps/fake-work)
function(target_add_fake_work TARGET)
    target_include_directories(${TARGET} PUBLIC ${FAKE_WORK_DIR})
    target_link_libraries(${TARGET} ${FAKE_WORK_DIR}/libfakework.a)
endfunction(target_add_fake_work)

set(DPDK_DIR ${CMAKE_SOURCE_DIR}/../afp/deps/dpdk)
set(ENV{PKG_CONFIG_PATH} "${DPDK_DIR}/build/lib/x86_64-linux-gnu/pkgconfig/")
execute_process(
    COMMAND pkg-config --cflags libdpdk
    OUTPUT_VARIABLE DPDK_CFLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND pkg-config --libs --static libdpdk
    OUTPUT_VARIABLE DPDK_LDFLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

function(target_add_dpdk TARGET)
    target_include_directories(${TARGET} PUBLIC ${DPDK_DIR}/build/include/)
    set_target_properties(${TARGET} PROPERTIES
        COMPILE_FLAGS ${DPDK_CFLAGS}
        LINK_FLAGS ${DPDK_LDFLAGS}
    )
endfunction(target_add_dpdk)

# RocksDB flags
set(RDB_DIR ${CMAKE_SOURCE_DIR}/../afp/deps/rocksdb)
set(RDB_LIBRARIES gflags snappy z lz4 bz2 zstd)
function(target_add_rocksdb TARGET)
    target_include_directories(${TARGET} PUBLIC ${RDB_DIR}/include/rocksdb)
    target_link_libraries(${TARGET} ${RDB_DIR}/librocksdb.a)
    target_link_libraries(${TARGET} ${RDB_LIBRARIES})
endfunction(target_add_rocksdb)

# default CFLAGS
set(COMMON_CFLAGS "-ggdb -fstack-protector -fPIC -Wall -Wextra -Wno-deprecated-declarations -Wno-uninitialized -Wno-unused-parameter -Wno-register -Wl,-zdefs -fvisibility=hidden -I/usr/include/boost -Wno-unused-variable -Wno-empty-body")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_CFLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_CFLAGS} -std=c++17 -D__STDC_LIMIT_MACROS")

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_subdirectory(src)
