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
 * \file
 *   This file contains the source code for the Sample App.
 */

/*
** Include Files:
*/
#include <string.h>

#include "sample_app.h"
#include "sample_app_cmds.h"
#include "sample_app_utils.h"
#include "sample_app_events.h"
#include "sample_app_version.h"
#include "sample_app_table.h"

/*
** global data
*/
SAMPLE_APP_Data_t SAMPLE_APP_Data;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
/*                                                                            */
/* Application entry point and main process loop                              */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
void SAMPLE_APP_Main(void)
{
    int32            status;
    CFE_SB_Buffer_t *SBBufPtr;

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(SAMPLE_APP_PERF_ID);

    /*
    ** Perform application-specific initialization
    ** If the Initialization fails, set the RunStatus to
    ** CFE_ES_RunStatus_APP_ERROR and the App will not enter the RunLoop
    */
    status = SAMPLE_APP_Init();
    if (status != CFE_SUCCESS)
    {
        SAMPLE_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    /*
    ** Sample App Runloop
    */
    while (CFE_ES_RunLoop(&SAMPLE_APP_Data.RunStatus) == true)
    {
        /*
        ** Performance Log Exit Stamp
        */
        CFE_ES_PerfLogExit(SAMPLE_APP_PERF_ID);

        /* Pend on receipt of command packet */
        status = CFE_SB_ReceiveBuffer(&SBBufPtr, SAMPLE_APP_Data.CommandPipe, CFE_SB_PEND_FOREVER);

        /*
        ** Performance Log Entry Stamp
        */
        CFE_ES_PerfLogEntry(SAMPLE_APP_PERF_ID);

        if (status == CFE_SUCCESS)
        {
            SAMPLE_APP_ProcessCommandPacket(SBBufPtr);
        }
        else
        {
            CFE_EVS_SendEvent(SAMPLE_APP_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "SAMPLE APP: SB Pipe Read Error, App Will Exit");

            SAMPLE_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }
    }

    /*
    ** Performance Log Exit Stamp
    */
    CFE_ES_PerfLogExit(SAMPLE_APP_PERF_ID);

    CFE_ES_ExitApp(SAMPLE_APP_Data.RunStatus);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* Initialization                                                             */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 SAMPLE_APP_Init(void)
{
    int32 status;

    /* Zero out the global data structure */
    memset(&SAMPLE_APP_Data, 0, sizeof(SAMPLE_APP_Data));

    SAMPLE_APP_Data.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Initialize app configuration data
    */
    SAMPLE_APP_Data.PipeDepth = SAMPLE_APP_PIPE_DEPTH;

    strncpy(SAMPLE_APP_Data.PipeName, "SAMPLE_APP_CMD_PIPE", sizeof(SAMPLE_APP_Data.PipeName));
    SAMPLE_APP_Data.PipeName[sizeof(SAMPLE_APP_Data.PipeName) - 1] = 0;

    /*
    ** Register the events
    */
    status = CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Sample App: Error Registering Events, RC = 0x%08lX\n", (unsigned long)status);
    }
    else
    {
        /*
         ** Initialize housekeeping packet (clear user data area).
         */
        CFE_MSG_Init(CFE_MSG_PTR(SAMPLE_APP_Data.HkTlm.TelemetryHeader), CFE_SB_ValueToMsgId(SAMPLE_APP_HK_TLM_MID),
                     sizeof(SAMPLE_APP_Data.HkTlm));

        /*
         ** Create Software Bus message pipe.
         */
        status = CFE_SB_CreatePipe(&SAMPLE_APP_Data.CommandPipe, SAMPLE_APP_Data.PipeDepth, SAMPLE_APP_Data.PipeName);
        if (status != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog("Sample App: Error creating pipe, RC = 0x%08lX\n", (unsigned long)status);
        }
    }

    if (status == CFE_SUCCESS)
    {
        /*
        ** Subscribe to Housekeeping request commands
        */
        status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(SAMPLE_APP_SEND_HK_MID), SAMPLE_APP_Data.CommandPipe);
        if (status != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog("Sample App: Error Subscribing to HK request, RC = 0x%08lX\n", (unsigned long)status);
        }
    }

    if (status == CFE_SUCCESS)
    {
        /*
        ** Subscribe to ground command packets
        */
        status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(SAMPLE_APP_CMD_MID), SAMPLE_APP_Data.CommandPipe);
        if (status != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog("Sample App: Error Subscribing to Command, RC = 0x%08lX\n", (unsigned long)status);
        }
    }

    if (status == CFE_SUCCESS)
    {
        /*
        ** Register Table(s)
        */
        status = CFE_TBL_Register(&SAMPLE_APP_Data.TblHandles[0], "SampleAppTable", sizeof(SAMPLE_APP_Table_t),
                                  CFE_TBL_OPT_DEFAULT, SAMPLE_APP_TblValidationFunc);
        if (status != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog("Sample App: Error Registering Table, RC = 0x%08lX\n", (unsigned long)status);
        }
        else
        {
            status = CFE_TBL_Load(SAMPLE_APP_Data.TblHandles[0], CFE_TBL_SRC_FILE, SAMPLE_APP_TABLE_FILE);
        }

        CFE_EVS_SendEvent(SAMPLE_APP_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION, "Sample App Initialized.%s",
                          SAMPLE_APP_VERSION_STRING);
    }

    return status;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/*  Purpose:                                                                  */
/*     This routine will process any packet that is received on the Sample    */
/*     App command pipe.                                                      */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void SAMPLE_APP_ProcessCommandPacket(CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;

    CFE_MSG_GetMsgId(&SBBufPtr->Msg, &MsgId);

    switch (CFE_SB_MsgIdToValue(MsgId))
    {
        case SAMPLE_APP_CMD_MID:
            SAMPLE_APP_ProcessGroundCommand(SBBufPtr);
            break;

        case SAMPLE_APP_SEND_HK_MID:
            SAMPLE_APP_ReportHousekeeping((CFE_MSG_CommandHeader_t *)SBBufPtr);
            break;

        default:
            CFE_EVS_SendEvent(SAMPLE_APP_MID_ERR_EID, CFE_EVS_EventType_ERROR,
                              "SAMPLE: invalid command packet,MID = 0x%x", (unsigned int)CFE_SB_MsgIdToValue(MsgId));
            break;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* Process Ground Commands                                                    */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void SAMPLE_APP_ProcessGroundCommand(CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_MSG_FcnCode_t CommandCode = 0;

    CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &CommandCode);

    /*
    ** Process "known" Sample App ground commands
    */
    switch (CommandCode)
    {
        case SAMPLE_APP_NOOP_CC:
            if (SAMPLE_APP_VerifyCmdLength(&SBBufPtr->Msg, sizeof(SAMPLE_APP_NoopCmd_t)))
            {
                SAMPLE_APP_Noop((SAMPLE_APP_NoopCmd_t *)SBBufPtr);
            }
            break;

        case SAMPLE_APP_RESET_COUNTERS_CC:
            if (SAMPLE_APP_VerifyCmdLength(&SBBufPtr->Msg, sizeof(SAMPLE_APP_ResetCountersCmd_t)))
            {
                SAMPLE_APP_ResetCounters((SAMPLE_APP_ResetCountersCmd_t *)SBBufPtr);
            }
            break;

        case SAMPLE_APP_PROCESS_CC:
            if (SAMPLE_APP_VerifyCmdLength(&SBBufPtr->Msg, sizeof(SAMPLE_APP_ProcessCmd_t)))
            {
                SAMPLE_APP_Process((SAMPLE_APP_ProcessCmd_t *)SBBufPtr);
            }
            break;

        /* default case already found during FC vs length test */
        default:
            CFE_EVS_SendEvent(SAMPLE_APP_CC_ERR_EID, CFE_EVS_EventType_ERROR, "Invalid ground command code: CC = %d",
                              CommandCode);
            break;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function is triggered in response to a task telemetry request */
/*         from the housekeeping task. This function will gather the App's    */
/*         telemetry, packetize it and send it to the housekeeping task via   */
/*         the software bus.                                                  */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 SAMPLE_APP_ReportHousekeeping(const CFE_MSG_CommandHeader_t *Msg)
{
    int i;

    /*
    ** Get command execution counters...
    */
    SAMPLE_APP_Data.HkTlm.Payload.CommandErrorCounter = SAMPLE_APP_Data.ErrCounter;
    SAMPLE_APP_Data.HkTlm.Payload.CommandCounter      = SAMPLE_APP_Data.CmdCounter;

    /*
    ** Send housekeeping telemetry packet...
    */
    CFE_SB_TimeStampMsg(CFE_MSG_PTR(SAMPLE_APP_Data.HkTlm.TelemetryHeader));
    CFE_SB_TransmitMsg(CFE_MSG_PTR(SAMPLE_APP_Data.HkTlm.TelemetryHeader), true);

    /*
    ** Manage any pending table loads, validations, etc.
    */
    for (i = 0; i < SAMPLE_APP_NUMBER_OF_TABLES; i++)
    {
        CFE_TBL_Manage(SAMPLE_APP_Data.TblHandles[i]);
    }

    return CFE_SUCCESS;
}
