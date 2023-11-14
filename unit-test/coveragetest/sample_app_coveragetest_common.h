/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

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

#include "setup.h"
#include "eventcheck.h"

#include "cfe.h"
#include "sample_app_eventids.h"
#include "sample_app.h"
#include "sample_app_dispatch.h"
#include "sample_app_cmds.h"
#include "sample_app_utils.h"
#include "sample_app_msgids.h"
#include "sample_app_msg.h"
#include "sample_app_tbl.h"

/*
 * Macro to add a test case to the list of tests to execute
 */
#define ADD_TEST(test) UtTest_Add((Test_##test), Sample_UT_Setup, Sample_UT_TearDown, #test)

#endif /* SAMPLE_APP_COVERAGETEST_COMMON_H */
