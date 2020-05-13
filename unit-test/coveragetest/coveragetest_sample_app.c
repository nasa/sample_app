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

void Test_SAMPLE_AppMain(void)
{
    /*
     * Test Case For:
     * void SAMPLE_AppMain( void )
     */

    UT_CheckEvent_t EventTest;

    /*
     * SAMPLE_AppMain does not return a value,
     * but it has several internal decision points
     * that need to be exercised here.
     *
     * First call it in "nominal" mode where all
     * dependent calls should be successful by default.
     */
    SAMPLE_AppMain();

    /*
     * Confirm that CFE_ES_ExitApp() was called at the end of execution
     */
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_ExitApp)) == 1,
            "CFE_ES_ExitApp() called");

    /*
     * Now set up individual cases for each of the error paths.
     * The first is for SAMPLE_AppInit().  As this is in the same
     * code unit, it is not a stub where the return code can be
     * easily set.  In order to get this to fail, an underlying
     * call needs to fail, and the error gets propagated through.
     * The call to CFE_EVS_Register is the first opportunity.
     * Any identifiable (non-success) return code should work.
     */
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_Register), 1,
            CFE_EVS_INVALID_PARAMETER);

    /*
     * Just call the function again.  It does not return
     * the value, so there is nothing to test for here directly.
     * However, it should show up in the coverage report that
     * the SAMPLE_AppInit() failure path was taken.
     */
    SAMPLE_AppMain();

    /*
     * This can validate that the internal "RunStatus" was
     * set to CFE_ES_RunStatus_APP_ERROR, by querying the struct directly.
     *
     * It is always advisable to include the _actual_ values
     * when asserting on conditions, so if/when it fails, the
     * log will show what the incorrect value was.
     */
    UtAssert_True(SAMPLE_AppData.RunStatus == CFE_ES_RunStatus_APP_ERROR,
            "SAMPLE_AppData.RunStatus (%lu) == CFE_ES_RunStatus_APP_ERROR",
            (unsigned long)SAMPLE_AppData.RunStatus);


    /*
     * Note that CFE_ES_RunLoop returns a boolean value,
     * so in order to exercise the internal "while" loop,
     * it needs to return TRUE.  But this also needs to return
     * FALSE in order to get out of the loop, otherwise
     * it will stay there infinitely.
     *
     * The deferred retcode will accomplish this.
     */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);

    /*
     * Invoke again
     */
    SAMPLE_AppMain();

    /*
     * Confirm that CFE_SB_RcvMsg() (inside the loop) was called
     */
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_SB_RcvMsg)) == 1,
            "CFE_SB_RcvMsg() called");

    /*
     * Now also make the CFE_SB_RcvMsg call fail,
     * to exercise that error path.  This sends an
     * event which can be checked with a hook function.
     */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_RcvMsg), 1, CFE_SB_PIPE_RD_ERR);
    UT_CheckEvent_Setup(&EventTest, SAMPLE_PIPE_ERR_EID);

    /*
     * Invoke again
     */
    SAMPLE_AppMain();

    /*
     * Confirm that the event was generated
     */
    UtAssert_True(EventTest.MatchCount == 1,
            "SAMPLE_PIPE_ERR_EID generated (%u)",
            (unsigned int)EventTest.MatchCount);
}

void Test_SAMPLE_AppInit(void)
{
    /*
     * Test Case For:
     * int32 SAMPLE_AppInit( void )
     */

    UT_CheckEvent_t EventTest;
    
    /* nominal case should return CFE_SUCCESS */
    UT_CheckEvent_Setup(&EventTest, SAMPLE_STARTUP_INF_EID);
    UT_TEST_FUNCTION_RC(SAMPLE_AppInit(), CFE_SUCCESS);
    UtAssert_True(EventTest.MatchCount == 1,
            "SAMPLE_STARTUP_INF_EID generated (%u)",
            (unsigned int)EventTest.MatchCount);


    /* trigger a failure for each of the sub-calls,
     * and confirm a write to syslog for each.
     * Note that this count accumulates, because the status
     * is _not_ reset between these test cases. */
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_Register), 1, CFE_EVS_INVALID_PARAMETER);
    UT_TEST_FUNCTION_RC(SAMPLE_AppInit(), CFE_EVS_INVALID_PARAMETER);
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 1,
            "CFE_ES_WriteToSysLog() called");


    UT_SetDeferredRetcode(UT_KEY(CFE_SB_CreatePipe), 1, CFE_SB_BAD_ARGUMENT);
    UT_CheckEvent_Setup(&EventTest, SAMPLE_APP_PIPE_CREATE_ERR_EID);
    UT_TEST_FUNCTION_RC(SAMPLE_AppInit(), CFE_SB_BAD_ARGUMENT);
    UtAssert_True(EventTest.MatchCount == 1,
            "SAMPLE_APP_PIPE_CREATE_ERR_EID generated (%u)",
            (unsigned int)EventTest.MatchCount);


    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), 1, CFE_SB_BAD_ARGUMENT);
    UT_CheckEvent_Setup(&EventTest, SAMPLE_APP_SUB_HK_ERR_EID);
    UT_TEST_FUNCTION_RC(SAMPLE_AppInit(), CFE_SB_BAD_ARGUMENT);
    UtAssert_True(EventTest.MatchCount == 1,
            "SAMPLE_APP_SUB_HK_ERR_EID generated (%u)",
            (unsigned int)EventTest.MatchCount);


    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), 2, CFE_SB_BAD_ARGUMENT);
    UT_CheckEvent_Setup(&EventTest, SAMPLE_APP_SUB_CMD_ERR_EID);
    UT_TEST_FUNCTION_RC(SAMPLE_AppInit(), CFE_SB_BAD_ARGUMENT);
    UtAssert_True(EventTest.MatchCount == 1,
            "SAMPLE_APP_SUB_CMD_ERR_EID generated (%u)",
            (unsigned int)EventTest.MatchCount);


    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 1, CFE_TBL_ERR_INVALID_OPTIONS);
    UT_CheckEvent_Setup(&EventTest, SAMPLE_APP_TBL_REG_ERR_EID);
    UT_TEST_FUNCTION_RC(SAMPLE_AppInit(), CFE_TBL_ERR_INVALID_OPTIONS);
    UtAssert_True(EventTest.MatchCount == 1,
            "SAMPLE_APP_TBL_REG_ERR_EID generated (%u)",
            (unsigned int)EventTest.MatchCount);

}

void Test_SAMPLE_ProcessCommandPacket(void)
{
    /*
     * Test Case For:
     * void SAMPLE_ProcessCommandPacket( CFE_SB_MsgPtr_t Msg )
     */
    /* a buffer large enough for any command message */
    union
    {
        CFE_SB_Msg_t Base;
        CCSDS_CommandPacket_t Cmd;
        SAMPLE_Noop_t Noop;
        SAMPLE_ResetCounters_t Reset;
        SAMPLE_Process_t Process;
    } TestMsg;
    CFE_SB_MsgId_t TestMsgId;
    UT_CheckEvent_t EventTest;

    memset(&TestMsg, 0, sizeof(TestMsg));
    UT_CheckEvent_Setup(&EventTest, SAMPLE_INVALID_MSGID_ERR_EID);

    /*
     * The CFE_SB_GetMsgId() stub uses a data buffer to hold the
     * message ID values to return.
     */
    TestMsgId = SAMPLE_APP_CMD_MID;
    UT_SetDataBuffer(UT_KEY(CFE_SB_GetMsgId), &TestMsgId,
            sizeof(TestMsgId), false);
    SAMPLE_ProcessCommandPacket(&TestMsg.Base);

    TestMsgId = SAMPLE_APP_SEND_HK_MID;
    UT_SetDataBuffer(UT_KEY(CFE_SB_GetMsgId), &TestMsgId,
            sizeof(TestMsgId), false);
    SAMPLE_ProcessCommandPacket(&TestMsg.Base);

    /* invalid message id */
    TestMsgId = CFE_SB_INVALID_MSG_ID;
    UT_SetDataBuffer(UT_KEY(CFE_SB_GetMsgId), &TestMsgId,
            sizeof(TestMsgId), false);
    SAMPLE_ProcessCommandPacket(&TestMsg.Base);


    /*
     * Confirm that the event was generated only _once_
     */
    UtAssert_True(EventTest.MatchCount == 1,
            "SAMPLE_INVALID_MSGID_ERR_EID generated (%u)",
            (unsigned int)EventTest.MatchCount);
}


void Test_SAMPLE_ProcessGroundCommand(void)
{
    /*
     * Test Case For:
     * void SAMPLE_ProcessGroundCommand( CFE_SB_MsgPtr_t Msg )
     */

    /* a buffer large enough for any command message */
    union
    {
        CFE_SB_Msg_t Base;
        CCSDS_CommandPacket_t Cmd;
        SAMPLE_Noop_t Noop;
        SAMPLE_ResetCounters_t Reset;
        SAMPLE_Process_t Process;
    } TestMsg;
    UT_CheckEvent_t EventTest;

    memset(&TestMsg, 0, sizeof(TestMsg));

    /*
     * call with each of the supported command codes
     * The CFE_SB_GetCmdCode stub allows the code to be
     * set to whatever is needed.  There is no return
     * value here and the actual implementation of these
     * commands have separate test cases, so this just
     * needs to exercise the "switch" statement.
     */

    /* test dispatch of NOOP */
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_GetCmdCode), 1, SAMPLE_APP_NOOP_CC);
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_GetTotalMsgLength), 1, sizeof(TestMsg.Noop));
    UT_CheckEvent_Setup(&EventTest, SAMPLE_COMMANDNOP_INF_EID);

    SAMPLE_ProcessGroundCommand(&TestMsg.Base);

    /* test dispatch of RESET */
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_GetCmdCode), 1, SAMPLE_APP_RESET_COUNTERS_CC);
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_GetTotalMsgLength), 1, sizeof(TestMsg.Reset));
    UT_CheckEvent_Setup(&EventTest, SAMPLE_COMMANDRST_INF_EID);

    SAMPLE_ProcessGroundCommand(&TestMsg.Base);

    /* test dispatch of PROCESS */
    /* note this will end up calling SAMPLE_Process(), and as such it needs to
     * avoid dereferencing a table which does not exist. */
    UT_SetForceFail(UT_KEY(CFE_TBL_GetAddress), CFE_TBL_ERR_UNREGISTERED);
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_GetCmdCode), 1, SAMPLE_APP_PROCESS_CC);
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_GetTotalMsgLength), 1, sizeof(TestMsg.Process));

    SAMPLE_ProcessGroundCommand(&TestMsg.Base);

    /* test an invalid CC */
    UT_CheckEvent_Setup(&EventTest, SAMPLE_COMMAND_ERR_EID);
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_GetCmdCode), 1, 1000);
    SAMPLE_ProcessGroundCommand(&TestMsg.Base);

    /*
     * Confirm that the event was generated only _once_
     */
    UtAssert_True(EventTest.MatchCount == 1,
            "SAMPLE_COMMAND_ERR_EID generated (%u)",
            (unsigned int)EventTest.MatchCount);

}

void Test_SAMPLE_ReportHousekeeping(void)
{
    /*
     * Test Case For:
     * void SAMPLE_ReportHousekeeping( const CCSDS_CommandPacket_t *Msg )
     */
    CCSDS_CommandPacket_t   CmdMsg;
    SAMPLE_HkTlm_t          HkTelemetryMsg;

    memset(&CmdMsg, 0, sizeof(CmdMsg));
    memset(&HkTelemetryMsg, 0, sizeof(HkTelemetryMsg));

    /*
     * Force CmdCounter and ErrCounter to known values
     */
    SAMPLE_AppData.CmdCounter = 22;
    SAMPLE_AppData.ErrCounter = 11;

    /*
     * CFE_SB_InitMsg() needs to be done to set the emulated MsgId and Length.
     *
     * The FSW code only does this once during init and relies on it
     * remaining during the SAMPLE_ReportHousekeeping().  This does
     * not happen during UT so it must be initialized again here.
     */
    CFE_SB_InitMsg(&SAMPLE_AppData.HkBuf.MsgHdr,
                   SAMPLE_APP_HK_TLM_MID,
                   sizeof(SAMPLE_AppData.HkBuf),
                   true);

    /*
     * Set up to "capture" the telemetry message
     */
    UT_SetDataBuffer(UT_KEY(CFE_SB_SendMsg), &HkTelemetryMsg,
            sizeof(HkTelemetryMsg), false);

    SAMPLE_ReportHousekeeping(&CmdMsg);

    /*
     * check that the known values got into the telemetry correctly
     */
    UtAssert_True(HkTelemetryMsg.Payload.CommandCounter == 22,
            "HkTelemetryMsg.Payload.CommandCounter (%u) == 22",
            (unsigned int)HkTelemetryMsg.Payload.CommandCounter);

    UtAssert_True(HkTelemetryMsg.Payload.CommandErrorCounter == 11,
            "HkTelemetryMsg.Payload.CommandErrorCounter (%u) == 11",
            (unsigned int)HkTelemetryMsg.Payload.CommandErrorCounter);

    /*
     * Confirm that the CFE_TBL_Manage() call was done
     */
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_TBL_Manage)) == 1,
            "CFE_TBL_Manage() called");

}

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
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_GetTotalMsgLength), 1,
                          sizeof(SAMPLE_Noop_t));
    UT_CheckEvent_Setup(&EventTest, SAMPLE_COMMANDNOP_INF_EID);

    SAMPLE_Noop(&TestMsg);

    /*
     * Confirm that the event was generated
     */
    UtAssert_True(EventTest.MatchCount == 1,
            "SAMPLE_COMMANDNOP_INF_EID generated (%u)",
            (unsigned int)EventTest.MatchCount);
}

void Test_SAMPLE_NoopCmd_LengthFailure(void)
{
    /*
     * Test Case For:
     * void SAMPLE_NoopCmd( const SAMPLE_Noop_t *Msg )
     */
    SAMPLE_Noop_t TestMsg;
    UT_CheckEvent_t EventTest;

    memset(&TestMsg, 0, sizeof(TestMsg));

    /* test NOOP with bad length */
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_GetTotalMsgLength), 1,
                          sizeof(SAMPLE_Noop_t) + 2);
    UT_CheckEvent_Setup(&EventTest, SAMPLE_COMMANDNOP_INF_EID);
    
    SAMPLE_ProcessGroundCommand(&TestMsg);

    /*
     * Confirm that the event was not generated
     */
    UtAssert_True(EventTest.MatchCount == 0,
            "SAMPLE_COMMANDNOP_INF_EID not generated (%u)",
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

    /*
     * Successful ResetCounters command - confirm that the event was generated
     */
    UT_CheckEvent_Setup(&EventTest, SAMPLE_COMMANDRST_INF_EID);

    UT_SetDeferredRetcode(UT_KEY(CFE_SB_GetTotalMsgLength), 1,
                          sizeof(SAMPLE_ResetCounters_t));


    SAMPLE_ResetCounters(&TestMsg);

    UtAssert_True(EventTest.MatchCount == 1,
            "SAMPLE_COMMANDRST_INF_EID generated (%u)",
            (unsigned int)EventTest.MatchCount);
}


void Test_SAMPLE_ResetCounters_LengthFailure(void)
{
    /*
     * Test Case For:
     * void SAMPLE_ResetCounters( const SAMPLE_ResetCounters_t *Msg )
     */
    SAMPLE_ResetCounters_t TestMsg;
    UT_CheckEvent_t EventTest;

    memset(&TestMsg, 0, sizeof(TestMsg));

    /*
     * Failed ResetCounters command - confirm that the event was not generated
     */
    UT_CheckEvent_Setup(&EventTest, SAMPLE_COMMANDRST_INF_EID);

    UT_SetDeferredRetcode(UT_KEY(CFE_SB_GetTotalMsgLength), 1,
                          sizeof(SAMPLE_ResetCounters_t) + 2);

    SAMPLE_ProcessGroundCommand(&TestMsg);

    UtAssert_True(EventTest.MatchCount == 0,
            "SAMPLE_COMMANDRST_INF_EID not generated (%u)",
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
    UT_CheckEvent_t EventTest;
    
    memset(&TestTblData, 0, sizeof(TestTblData));
    memset(&TestMsg, 0, sizeof(TestMsg));

    /* Provide some table data for the SAMPLE_Process() function to use */
    TestTblData.Int1 = 40;
    TestTblData.Int2 = 50;
    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &TblPtr, sizeof(TblPtr), false);
 
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_GetTotalMsgLength), 1,
                          sizeof(SAMPLE_Process_t));

    UT_CheckEvent_Setup(&EventTest, SAMPLE_APP_TBL_INF_EID);

    SAMPLE_Process(&TestMsg);

    /*
     * Confirm that the CFE_TBL_GetAddress() call was done
     */
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_TBL_GetAddress)) == 1,
            "CFE_TBL_GetAddress() called");

    /*
     * Confirm that the Info event was sent.
     */
    UtAssert_True(EventTest.MatchCount == 1,
            "SAMPLE_APP_TBL_INF_EID generated (%u)",
            (unsigned int)EventTest.MatchCount);

    /*
     * Confirm that the SAMPLE_Function() call was done
     * NOTE: This stub is provided by the sample_lib library
     */
    UtAssert_True(UT_GetStubCount(UT_KEY(SAMPLE_Function)) == 1,
            "SAMPLE_Function() called");

}

void Test_SAMPLE_ProcessCC_LengthFailure(void)
{
    /*
     * Test Case For:
     * void  SAMPLE_ProcessCC( const SAMPLE_Process_t *Msg )
     */
    SAMPLE_Process_t TestMsg;
    SAMPLE_Table_t TestTblData;
    void *TblPtr = &TestTblData;
    UT_CheckEvent_t EventTest;
    
    memset(&TestTblData, 0, sizeof(TestTblData));
    memset(&TestMsg, 0, sizeof(TestMsg));

    /* Provide some table data for the SAMPLE_Process() function to use */
    TestTblData.Int1 = 40;
    TestTblData.Int2 = 50;
    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &TblPtr, sizeof(TblPtr), false);
 
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_GetTotalMsgLength), 1,
                          sizeof(SAMPLE_Process_t) + 2);

    UT_CheckEvent_Setup(&EventTest, SAMPLE_APP_TBL_INF_EID);

    
    SAMPLE_ProcessGroundCommand(&TestMsg);

    /*
     * Confirm that the Info event was not sent.
     */
    UtAssert_True(EventTest.MatchCount == 0,
            "SAMPLE_APP_TBL_INF_EID generated (%u)",
            (unsigned int)EventTest.MatchCount);

    /*
     * Confirm that the SAMPLE_Function and CFE_TBL_ReleaseAddress() 
     * calls were not done
     */
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_TBL_GetAddress)) == 0,
            "CFE_TBL_GetAddress() not called");
    UtAssert_True(UT_GetStubCount(UT_KEY(SAMPLE_Function)) == 0,
            "SAMPLE_Function() not called");


}

void Test_SAMPLE_ProcessCC_TblRegisterFailure(void)
{
    /*
     * Test Case For:
     * void  SAMPLE_ProcessCC( const SAMPLE_Process_t *Msg )
     */
    SAMPLE_Process_t TestMsg;
    SAMPLE_Table_t TestTblData;
    void *TblPtr = &TestTblData;
    UT_CheckEvent_t EventTest;
    
    memset(&TestTblData, 0, sizeof(TestTblData));
    memset(&TestMsg, 0, sizeof(TestMsg));

    /* Provide some table data for the SAMPLE_Process() function to use */
    TestTblData.Int1 = 40;
    TestTblData.Int2 = 50;
    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &TblPtr, sizeof(TblPtr), false);
 
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_GetTotalMsgLength), 1,
                          sizeof(SAMPLE_Process_t));

    UT_CheckEvent_Setup(&EventTest, SAMPLE_APP_TBL_ADDR_ERR_EID);

     /*
     * Configure the CFE_TBL_GetAddress function to return an error
     * Exercise the error return path
     */
    UT_SetForceFail(UT_KEY(CFE_TBL_GetAddress), CFE_TBL_ERR_UNREGISTERED);
 
    SAMPLE_Process(&TestMsg);

    /*
     * Confirm that the SAMPLE_Function and CFE_TBL_ReleaseAddress() 
     * calls were not done
     */
    UtAssert_True(UT_GetStubCount(UT_KEY(CFE_TBL_ReleaseAddress)) == 0,
            "CFE_TBL_ReleaseAddress() not called");
    UtAssert_True(UT_GetStubCount(UT_KEY(SAMPLE_Function)) == 0,
            "SAMPLE_Function() not called");

    /*
     * Confirm that the Error event was sent.
     */
    UtAssert_True(EventTest.MatchCount == 1,
            "SAMPLE_APP_TBL_ADDR_ERR_EID generated (%u)",
            (unsigned int)EventTest.MatchCount);

}

void Test_SAMPLE_ProcessCC_TblReleaseFailure(void)
{
    /*
     * Test Case For:
     * void  SAMPLE_ProcessCC( const SAMPLE_Process_t *Msg )
     */
    SAMPLE_Process_t TestMsg;
    SAMPLE_Table_t TestTblData;
    void *TblPtr = &TestTblData;
    UT_CheckEvent_t EventTest;
    
    memset(&TestTblData, 0, sizeof(TestTblData));
    memset(&TestMsg, 0, sizeof(TestMsg));

    /* Provide some table data for the SAMPLE_Process() function to use */
    TestTblData.Int1 = 40;
    TestTblData.Int2 = 50;
    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &TblPtr, sizeof(TblPtr), false);
 
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_GetTotalMsgLength), 1,
                          sizeof(SAMPLE_Process_t));

    UT_CheckEvent_Setup(&EventTest, SAMPLE_APP_TBL_REL_ERR_EID);

     /*
     * Configure the CFE_TBL_ReleaseAddress function to return an error
     * Exercise the error return path
     */
    UT_SetForceFail(UT_KEY(CFE_TBL_ReleaseAddress), CFE_TBL_ERR_UNREGISTERED);
 
    SAMPLE_Process(&TestMsg);

    /*
     * Confirm that the SAMPLE_Function call was not done
     */
    UtAssert_True(UT_GetStubCount(UT_KEY(SAMPLE_Function)) == 0,
            "SAMPLE_Function() not called");

    /*
     * Confirm that the Error event was sent.
     */
    UtAssert_True(EventTest.MatchCount == 1,
            "SAMPLE_APP_TBL_REL_ERR_EID generated (%u)",
            (unsigned int)EventTest.MatchCount);

}

void Test_SAMPLE_VerifyCmdLength(void)
{
    /*
     * Test Case For:
     * bool SAMPLE_VerifyCmdLength( CFE_SB_MsgPtr_t Msg, uint16 ExpectedLength )
     */
    CFE_SB_Msg_t TestMsg;
    UT_CheckEvent_t EventTest;

    memset(&TestMsg, 0, sizeof(TestMsg));

    UT_CheckEvent_Setup(&EventTest, SAMPLE_LEN_ERR_EID);

    /*
     * test a mismatch case
     */
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_GetTotalMsgLength), 1,
            10 + sizeof(TestMsg));
    SAMPLE_VerifyCmdLength(&TestMsg, sizeof(TestMsg));

    /*
     * Confirm that the error event generated
     */
    UtAssert_True(EventTest.MatchCount == 1,
            "SAMPLE_LEN_ERR_EID generated (%u)",
            (unsigned int)EventTest.MatchCount);
}

void Test_SAMPLE_VerifyCmdLength_Failure(void)
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

}


void Test_SAMPLE_TblValidationFunc_Failure(void)
{
    /*
     * Test Case For:
     * int32 SAMPLE_TblValidationFunc( void *TblData )
     */
    SAMPLE_Table_t TestTblData;

    memset(&TestTblData, 0, sizeof(TestTblData));

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
    UT_CheckEvent_t EventTest;

    UT_ClearForceFail(UT_KEY(CFE_TBL_GetInfo));
    UT_CheckEvent_Setup(&EventTest, SAMPLE_APP_TBL_CRC_INF_EID);
    SAMPLE_GetCrc("UT");
    UtAssert_True(EventTest.MatchCount == 1,
            "SAMPLE_APP_TBL_CRC_INF_EID generated (%u)",
            (unsigned int)EventTest.MatchCount);
}

void Test_SAMPLE_GetCrc_Failure(void)
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
    UT_CheckEvent_t EventTest;


    UT_SetForceFail(UT_KEY(CFE_TBL_GetInfo), CFE_TBL_ERR_INVALID_NAME);
    UT_CheckEvent_Setup(&EventTest, SAMPLE_APP_TBL_GETINF_ERR_EID);
    SAMPLE_GetCrc("UT");
    UtAssert_True(EventTest.MatchCount == 1,
            "SAMPLE_APP_TBL_GETINF_ERR_EID generated (%u)",
            (unsigned int)EventTest.MatchCount);
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
    ADD_TEST(SAMPLE_AppMain);
    ADD_TEST(SAMPLE_AppInit);
    ADD_TEST(SAMPLE_ProcessCommandPacket);
    ADD_TEST(SAMPLE_ProcessGroundCommand);
    ADD_TEST(SAMPLE_ReportHousekeeping);
    ADD_TEST(SAMPLE_NoopCmd);
    ADD_TEST(SAMPLE_NoopCmd_LengthFailure);
    ADD_TEST(SAMPLE_ResetCounters);
    ADD_TEST(SAMPLE_ResetCounters_LengthFailure);
    ADD_TEST(SAMPLE_ProcessCC);
    ADD_TEST(SAMPLE_ProcessCC_LengthFailure);
    ADD_TEST(SAMPLE_ProcessCC_TblRegisterFailure);
    ADD_TEST(SAMPLE_ProcessCC_TblReleaseFailure);
    ADD_TEST(SAMPLE_VerifyCmdLength);
    ADD_TEST(SAMPLE_VerifyCmdLength_Failure);
    ADD_TEST(SAMPLE_TblValidationFunc);
    ADD_TEST(SAMPLE_TblValidationFunc_Failure);
    ADD_TEST(SAMPLE_GetCrc);
    ADD_TEST(SAMPLE_GetCrc_Failure);
}





