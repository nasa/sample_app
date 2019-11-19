## How To setup environment (cmake) for coverage test

The minimum needed to setup (CMakeLists) for coverage test:

1. UT_Assert framework
2. Your module and any custom stubs
3. CFE stubs
4. BSP stubs
5. Osapi stubs
6. PSP stubs (as needed)
7. CFS Header files

To run cmake file for coverage test:

1. cmake .
2. make
3. make test

See Below for example.

```
cmake_minimum_required(VERSION 2.6.4)
project(CFS_FS_UNTAR_COVERAGE C)

# Ask to generate a "make test" target
enable_testing()

add_definitions(-Wall -Werror)
add_definitions(-D_UNIT_TEST_)

set(CFS_MAIN_DIR "../../../..")
set(osal_path "${CFS_MAIN_DIR}/osal")
set(cfe_path "${CFS_MAIN_DIR}/cfe")
set(psp_path "${CFS_MAIN_DIR}/psp")
set(build_path "${CFS_MAIN_DIR}/build")

#Set BSPTYPE
set(BSPTYPE "pc-linux")

#include Header for assert framework
include_directories(${osal_path}/ut_assert/inc)
include_directories(${osal_path}/ut_assert/inc)

#include program to test
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

#include stub
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../public_inc) #Update

#include CFE Headers
set(INCLUDE_FILES ${osal_path}/src/os/inc
                  ${psp_path}/fsw/inc
                  ${osal_path}/src/bsp/${BSPTYPE}/config
                  ${cfe_path}/fsw/cfe-core/src/inc
                  ${cfe_path}/fsw/cfe-core/src/time
                  ${cfe_path}/fsw/cfe-core/src/sb
                  ${cfe_path}/fsw/cfe-core/src/es
                  ${cfe_path}/fsw/cfe-core/src/evs
                  ${cfe_path}/fsw/cfe-core/src/fs
                  ${cfe_path}/fsw/cfe-core/src/tbl
                  ${cfe_path}/fsw/mission_inc
                  ${cfe_path}/fsw/platform_inc/cpu1
                  ${cfe_path}/cmake/target/inc)

include_directories(${INCLUDE_FILES})

# These include paths are not public APIs but are needed
# to get to some internal headers for private APIs
include_directories(${osal_path}/src/os/shared)

#add source path UT_Assert, UT_OSAPI_STUB, UT_CFE_STUB, BSPFILES
aux_source_directory(${osal_path}/src/ut-stubs UT_OSAPI_STUB_FILES)
aux_source_directory(${cfe_path}/fsw/cfe-core/unit-test  UT_CFE_STUB_FILES)
aux_source_directory(${osal_path}/src/bsp/${BSPTYPE}/ut-src BSPFILES)
aux_source_directory(${osal_path}/ut_assert/src UT_ASSERT_FILES)

#add library
add_library(ut_osapi_stubs STATIC ${UT_OSAPI_STUB_FILES})
add_library(ut_cfe_stubs STATIC ${UT_CFE_STUB_FILES})
add_library(ut_bsp STATIC ${BSPFILES})
add_library(ut_assert STATIC ${UT_ASSERT_FILES})

#Create Executable
add_executable(cfs_untar_coverage_test unit-test-coverage/cfs_untar_coveragetest.c)
target_link_libraries (cfs_untar_coverage_test ut_cfe_stubs
                                               ut_bsp
                                               ut_assert
                                               ut_osapi_stubs )

# Add test
ADD_TEST(cfs_untar_converage_test cfs_untar_coverage_test)
```

### Note(s):

1. Points cfe_platform_cfg.h toward cpu1_platform_cfg.h, cpu1_platform_cfg points toward your mission configuration.
