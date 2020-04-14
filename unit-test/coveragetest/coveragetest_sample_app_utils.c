/*
**  GSC-18128-1, "Core Flight Executive Version 6.7"
**
**  Copyright (c) 2006-2019 United States Government as represented by
**  the Administrator of the National Aeronautics and Space Administration.
**  All Rights Reserved.
**
**  Licensed under the Apache License, Version 2.0 (the "License");
**  you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
**  Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissions and
**  limitations under the License.
*/

/*
** File: coveragetest_sample_app.c
**
** Purpose:
** Coverage Unit Test cases for the SAMPLE Application
**
** Notes:
** This implements various test cases to exercise all code
** paths through all functions defined in the SAMPLE application.
**
** It is primarily focused at providing examples of the various
** stub configurations, hook functions, and wrapper calls that
** are often needed when coercing certain code paths through
** complex functions.
*/


/*
 * Includes
 */

#include "sample_lib.h"
#include "sample_app_coveragetest_common.h"
#include "ut_sample_app.h"

/* to get the SAMPLE_Function() declaration */

typedef struct
{
    uint16 ExpectedEvent;
    uint32 MatchCount;
} UT_CheckEvent_t;

/*
 * An example hook function to check for a specific event.
 */
static int32 UT_CheckEvent_Hook(void *UserObj, int32 StubRetcode,
        uint32 CallCount, const UT_StubContext_t *Context)
{
    UT_CheckEvent_t *State = UserObj;
    uint16 *EventIdPtr;

    /*
     * The CFE_EVS_SendEvent stub passes the EventID as the
     * first context argument.
     */
    if (Context->ArgCount > 0)
    {
        EventIdPtr = (uint16*)Context->ArgPtr[0];
        if (*EventIdPtr == State->ExpectedEvent)
        {
            ++State->MatchCount;
        }
    }

    return 0;
}

/*
 * Helper function to set up for event checking
 * This attaches the hook function to CFE_EVS_SendEvent
 */
static void UT_CheckEvent_Setup(UT_CheckEvent_t *Evt, uint16 ExpectedEvent)
{
    memset(Evt, 0, sizeof(*Evt));
    Evt->ExpectedEvent = ExpectedEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_CheckEvent_Hook, Evt);
}




/*
**********************************************************************************
**          TEST CASE FUNCTIONS
**********************************************************************************
*/
void Test_SAMPLE_VerifyCmdLength(void)
{
    /*
     * Test Case For:
     * bool SAMPLE_VerifyCmdLength( CFE_SB_MsgPtr_t Msg, uint16 ExpectedLength )
     */
    CFE_SB_Msg_t TestMsg;
    UT_CheckEvent_t EventTest;

    memset(&TestMsg, 0, sizeof(TestMsg));

    /*
     * test a match case
     */
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_GetTotalMsgLength), 1, sizeof(TestMsg));
    UT_CheckEvent_Setup(&EventTest, SAMPLE_LEN_ERR_EID);

    SAMPLE_VerifyCmdLength(&TestMsg, sizeof(TestMsg));

    /*
     * Confirm that the event was NOT generated
     */
    UtAssert_True(EventTest.MatchCount == 0,
            "SAMPLE_LEN_ERR_EID NOT generated (%u)",
            (unsigned int)EventTest.MatchCount);

    /*
     * test a mismatch case
     */
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_GetTotalMsgLength), 1,
            10 + sizeof(TestMsg));
    SAMPLE_VerifyCmdLength(&TestMsg, sizeof(TestMsg));

    /*
     * Confirm that the event WAS generated
     */
    UtAssert_True(EventTest.MatchCount == 1,
            "SAMPLE_LEN_ERR_EID generated (%u)",
            (unsigned int)EventTest.MatchCount);
}

void Test_SAMPLE_TblValidationFunc(void)
{
    /*
     * Test Case For:
     * int32 SAMPLE_TblValidationFunc( void *TblData )
     */
    SAMPLE_Table_t TestTblData;

    memset(&TestTblData, 0, sizeof(TestTblData));

    /* nominal case (0) should succeed */
    UT_TEST_FUNCTION_RC(SAMPLE_TblValidationFunc(&TestTblData), CFE_SUCCESS);

    /* error case should return SAMPLE_TABLE_OUT_OF_RANGE_ERR_CODE */
    TestTblData.Int1 = 1 + SAMPLE_TBL_ELEMENT_1_MAX;
    UT_TEST_FUNCTION_RC(SAMPLE_TblValidationFunc(&TestTblData),
            SAMPLE_TABLE_OUT_OF_RANGE_ERR_CODE);
}

void Test_SAMPLE_GetCrc(void)
{
    /*
     * Test Case For:
     * void SAMPLE_GetCrc( const char *TableName )
     */

    /*
     * The only branch point here is CFE_TBL_GetInfo()
     *
     * Either way this function just does a write to syslog,
     * and it is the same in both cases, just with
     * a different message.  This could actually verify
     * the message using a hook function, if desired.
     */

    UT_SetForceFail(UT_KEY(CFE_TBL_GetInfo), CFE_TBL_ERR_INVALID_NAME);
    SAMPLE_GetCrc("UT");
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 1,
            "CFE_ES_WriteToSysLog() called");

    UT_ClearForceFail(UT_KEY(CFE_TBL_GetInfo));
    SAMPLE_GetCrc("UT");
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 2,
            "CFE_ES_WriteToSysLog() called");

}



/*
 * Setup function prior to every test
 */
void Sample_UT_Setup(void)
{
    UT_ResetState(0);
}

/*
 * Teardown function after every test
 */
void Sample_UT_TearDown(void)
{

}


/*
 * Register the test cases to execute with the unit test tool
 */
void UtTest_Setup(void)
{
    ADD_TEST(SAMPLE_VerifyCmdLength);
    ADD_TEST(SAMPLE_TblValidationFunc);
    ADD_TEST(SAMPLE_GetCrc);
}





