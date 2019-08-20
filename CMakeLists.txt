cmake_minimum_required(VERSION 2.6.4)
project(CFE_SAMPLE_APP C)

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(../../cfe/fsw/cfe-core/src/inc)
include_directories(../../osal/src/os/inc)
include_directories(../../osal/build/inc)
include_directories(../../build/editor_inc)
include_directories(../../psp/fsw/inc)
include_directories(../../build/tools/elf2cfetbl/inc_stubs)

aux_source_directory(fsw/src APP_SRC_FILES)

include(../../cfe/cmake/arch_build.cmake)

# Create the app module
add_cfe_app(sample_app ${APP_SRC_FILES})

