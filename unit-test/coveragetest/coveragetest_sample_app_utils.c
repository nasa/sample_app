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

/*
** File: coveragetest_sample_app.c
**
** Purpose:
** Coverage Unit Test cases for the Sample Application
**
** Notes:
** This implements various test cases to exercise all code
** paths through all functions defined in the Sample application.
**
** It is primarily focused at providing examples of the various
** stub configurations, hook functions, and wrapper calls that
** are often needed when coercing certain code paths through
** complex functions.
*/

/*
 * Includes
 */
#include "sample_lib.h" /* For SAMPLE_LIB_Function */
#include "sample_app_coveragetest_common.h"

/*
**********************************************************************************
**          TEST CASE FUNCTIONS
**********************************************************************************
*/

void Test_SAMPLE_APP_TblValidationFunc(void)
{
    /*
     * Test Case For:
     * CFE_Status_t SAMPLE_APP_TblValidationFunc( void *TblData )
     */
    SAMPLE_APP_ExampleTable_t TestTblData;

    memset(&TestTblData, 0, sizeof(TestTblData));

    /* nominal case (0) should succeed */
    UtAssert_INT32_EQ(SAMPLE_APP_TblValidationFunc(&TestTblData), CFE_SUCCESS);

    /* error case should return SAMPLE_APP_TABLE_OUT_OF_RANGE_ERR_CODE */
    TestTblData.Int1 = 1 + SAMPLE_APP_TBL_ELEMENT_1_MAX;
    UtAssert_INT32_EQ(SAMPLE_APP_TblValidationFunc(&TestTblData), SAMPLE_APP_TABLE_OUT_OF_RANGE_ERR_CODE);
}

void Test_SAMPLE_APP_GetCrc(void)
{
    /*
     * Test Case For:
     * void SAMPLE_APP_GetCrc( const char *TableName )
     */

    /*
     * The only branch point here is CFE_TBL_GetInfo()
     *
     * Either way this function just does a write to syslog,
     * and it is the same in both cases, just with
     * a different message.  This could actually verify
     * the message using a hook function, if desired.
     */

    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetInfo), CFE_TBL_ERR_INVALID_NAME);
    SAMPLE_APP_GetCrc("UT");
    UtAssert_STUB_COUNT(CFE_ES_WriteToSysLog, 1);

    UT_ClearDefaultReturnValue(UT_KEY(CFE_TBL_GetInfo));
    SAMPLE_APP_GetCrc("UT");
    UtAssert_STUB_COUNT(CFE_ES_WriteToSysLog, 2);
}

/*
 * Register the test cases to execute with the unit test tool
 */
void UtTest_Setup(void)
{
    ADD_TEST(SAMPLE_APP_TblValidationFunc);
    ADD_TEST(SAMPLE_APP_GetCrc);
}
