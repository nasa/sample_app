/**
 * @file
 *
 * Common definitions for all sample_app coverage tests
 */

#ifndef SAMPLE_APP_COVERAGETEST_COMMON_H
#define SAMPLE_APP_COVERAGETEST_COMMON_H

/*
 * Includes
 */

#include "utassert.h"
#include "uttest.h"
#include "utstubs.h"

#include "cfe.h"
#include "sample_app_events.h"
#include "sample_app.h"
#include "sample_app_table.h"

/*
 * Macro to add a test case to the list of tests to execute
 */
#define ADD_TEST(test) UtTest_Add((Test_##test), Sample_UT_Setup, Sample_UT_TearDown, #test)

/*
 * Setup function prior to every test
 */
void Sample_UT_Setup(void);

/*
 * Teardown function after every test
 */
void Sample_UT_TearDown(void);

#endif /* SAMPLE_APP_COVERAGETEST_COMMON_H */
