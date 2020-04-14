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
void Test_SAMPLE_NoopCmd(void)
{
    /*
     * Test Case For:
     * void SAMPLE_NoopCmd( const SAMPLE_Noop_t *Msg )
     */
    SAMPLE_Noop_t TestMsg;
    UT_CheckEvent_t EventTest;

    memset(&TestMsg, 0, sizeof(TestMsg));

    /* test dispatch of NOOP */
    UT_CheckEvent_Setup(&EventTest, SAMPLE_COMMANDNOP_INF_EID);

    SAMPLE_Noop(&TestMsg);

    /*
     * Confirm that the event was generated
     */
    UtAssert_True(EventTest.MatchCount == 1,
            "SAMPLE_COMMANDNOP_INF_EID generated (%u)",
            (unsigned int)EventTest.MatchCount);
}

void Test_SAMPLE_ResetCounters(void)
{
    /*
     * Test Case For:
     * void SAMPLE_ResetCounters( const SAMPLE_ResetCounters_t *Msg )
     */
    SAMPLE_ResetCounters_t TestMsg;
    UT_CheckEvent_t EventTest;

    memset(&TestMsg, 0, sizeof(TestMsg));

    UT_CheckEvent_Setup(&EventTest, SAMPLE_COMMANDRST_INF_EID);

    //UT_TEST_FUNCTION_RC(SAMPLE_ResetCounters(&TestMsg), CFE_SUCCESS);

    /*
     * Confirm that the event was generated
     */
    UtAssert_True(EventTest.MatchCount == 1,
            "SAMPLE_COMMANDRST_INF_EID generated (%u)",
            (unsigned int)EventTest.MatchCount);
}

void Test_SAMPLE_ProcessCC(void)
{
    /*
     * Test Case For:
     * void  SAMPLE_ProcessCC( const SAMPLE_Process_t *Msg )
     */
    SAMPLE_Process_t TestMsg;
    SAMPLE_Table_t TestTblData;
    void *TblPtr = &TestTblData;

    memset(&TestTblData, 0, sizeof(TestTblData));
    memset(&TestMsg, 0, sizeof(TestMsg));

    /* Provide some table data for the SAMPLE_Process() function to use */
    TestTblData.Int1 = 40;
    TestTblData.Int2 = 50;
    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &TblPtr, sizeof(TblPtr), false);
    //UT_TEST_FUNCTION_RC(SAMPLE_Process(&TestMsg), CFE_SUCCESS);

    /*
     * Confirm that the CFE_TBL_GetAddress() call was done
     */
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_TBL_GetAddress)) == 1,
            "CFE_TBL_GetAddress() called");


    /*
     * Confirm that the SAMPLE_Function() call was done
     * NOTE: This stub is provided by the sample_lib library
     */
    UtAssert_True(UT_GetStubCount(UT_KEY(SAMPLE_Function)) == 1,
            "SAMPLE_Function() called");

    /*
     * Configure the CFE_TBL_GetAddress function to return an error
     * Exercise the error return path
     */
    UT_SetForceFail(UT_KEY(CFE_TBL_GetAddress), CFE_TBL_ERR_UNREGISTERED);
    //UT_TEST_FUNCTION_RC(SAMPLE_Process(&TestMsg), CFE_TBL_ERR_UNREGISTERED);
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
    ADD_TEST(SAMPLE_NoopCmd);
    ADD_TEST(SAMPLE_ResetCounters);
    ADD_TEST(SAMPLE_ProcessCC);
}





