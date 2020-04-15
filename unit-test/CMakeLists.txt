##################################################################
#
# Coverage Unit Test build recipe
#
# This CMake file contains the recipe for building the sample unit tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

#
#
# NOTE on the subdirectory structures here:
#
# - "inc" provides local header files shared between the coveragetest,
#    wrappers, and overrides source code units
# - "coveragetest" contains source code for the actual unit test cases
#    The primary objective is to get line/path coverage on the FSW 
#    code units.
# - "wrappers" contains wrappers for the FSW code.  The wrapper adds
#    any UT-specific scaffolding to facilitate the coverage test, and
#    includes the unmodified FSW source file.
#
 
set(UT_NAME sample_app)

# Use the UT assert public API, and allow direct
# inclusion of source files that are normally private
include_directories(${osal_MISSION_DIR}/ut_assert/inc)
include_directories(${PROJECT_SOURCE_DIR}/fsw/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

set(TESTNAME "sample_app")
add_library(ut_${TESTNAME}_object OBJECT
       ${CFE_SAMPLE_APP_SOURCE_DIR}/fsw/src/sample_app.c
       ${CFE_SAMPLE_APP_SOURCE_DIR}/fsw/src/sample_app_cmds.c
       ${CFE_SAMPLE_APP_SOURCE_DIR}/fsw/src/sample_app_utils.c)

# Apply the UT_COVERAGE_COMPILE_FLAGS to the units under test
# This should enable coverage analysis on platforms that support this
target_compile_options(ut_${TESTNAME}_object PRIVATE
                     ${UT_COVERAGE_COMPILE_FLAGS})
        
# Compile a test runner application, which contains the
# actual coverage test code (test cases) and the unit under test
add_executable(${TESTNAME}-testrunner
     coveragetest/coveragetest_sample_app.c
     $<TARGET_OBJECTS:ut_${TESTNAME}_object>)
    
# This also needs to be linked with UT_COVERAGE_LINK_FLAGS (for coverage)
# This is also linked with any other stub libraries needed,
# as well as the UT assert framework    
target_link_libraries(${TESTNAME}-testrunner
        ${UT_COVERAGE_LINK_FLAGS}
        ut_sample_lib_stubs
        ut_cfe-core_stubs
        ut_assert
    )
    
# Add it to the set of tests to run as part of "make test"
add_test(${TESTNAME} ${TESTNAME}-testrunner)


