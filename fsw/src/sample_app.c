/*******************************************************************************
**
**      GSC-18128-1, "Core Flight Executive Version 6.7"
**
**      Copyright (c) 2006-2019 United States Government as represented by
**      the Administrator of the National Aeronautics and Space Administration.
**      All Rights Reserved.
**
**      Licensed under the Apache License, Version 2.0 (the "License");
**      you may not use this file except in compliance with the License.
**      You may obtain a copy of the License at
**
**        http://www.apache.org/licenses/LICENSE-2.0
**
**      Unless required by applicable law or agreed to in writing, software
**      distributed under the License is distributed on an "AS IS" BASIS,
**      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**      See the License for the specific language governing permissions and
**      limitations under the License.
**
** File: sample_app.c
**
** Purpose:
**   This file contains the source code for the Sample App.
**
*******************************************************************************/

/*
** Include Files:
*/
#include "sample_app_events.h"
#include "sample_app_version.h"
#include "sample_app.h"
#include "sample_table.h"
#include "sample_app_cmds.h"
#include "sample_app_utils.h"

/* The sample_lib module provides the SAMPLE_Function() prototype */
#include <string.h>
#include "sample_lib.h"

/*
** global data
*/
SAMPLE_AppData_t SAMPLE_AppData;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
/* SAMPLE_AppMain() -- Application entry point and main process loop          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
void SAMPLE_AppMain( void )
{
    int32  status;

    /*
    ** Register the app with Executive services
    */
    CFE_ES_RegisterApp();

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(SAMPLE_APP_PERF_ID);

    /*
    ** Perform application specific initialization
    ** If the Initialization fails, set the RunStatus to
    ** CFE_ES_RunStatus_APP_ERROR and the App will not enter the RunLoop
    */
    status = SAMPLE_AppInit();
    if (status != CFE_SUCCESS)
    {
        SAMPLE_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    /*
    ** SAMPLE Runloop
    */
    while (CFE_ES_RunLoop(&SAMPLE_AppData.RunStatus) == true)
    {
        /*
        ** Performance Log Exit Stamp
        */
        CFE_ES_PerfLogExit(SAMPLE_APP_PERF_ID);

        /* Pend on receipt of command packet */
        status = CFE_SB_RcvMsg(&SAMPLE_AppData.MsgPtr,
                               SAMPLE_AppData.CommandPipe,
                               CFE_SB_PEND_FOREVER);

        /*
        ** Performance Log Entry Stamp
        */
        CFE_ES_PerfLogEntry(SAMPLE_APP_PERF_ID);

        if (status == CFE_SUCCESS)
        {
            SAMPLE_ProcessCommandPacket(SAMPLE_AppData.MsgPtr);
        }
        else
        {
            CFE_EVS_SendEvent(SAMPLE_PIPE_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "SAMPLE APP: SB Pipe Read Error, App Will Exit");

            SAMPLE_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }

    }


    CFE_EVS_SendEvent(SAMPLE_APP_EXIT_ERR_EID,
                      CFE_EVS_EventType_ERROR,
                      "SAMPLE_APP Terminating");

    CFE_ES_WriteToSysLog("SAMPLE_APP Terminating.");

    /*
    ** Performance Log Exit Stamp
    */
    CFE_ES_PerfLogExit(SAMPLE_APP_PERF_ID);

    CFE_ES_ExitApp(SAMPLE_AppData.RunStatus);

} /* End of SAMPLE_AppMain() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* SAMPLE_AppInit() --  initialization                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 SAMPLE_AppInit( void )
{
    int32    status;

    SAMPLE_AppData.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Initialize app command execution counters
    */
    SAMPLE_AppData.CmdCounter = 0;
    SAMPLE_AppData.ErrCounter = 0;

    /*
    ** Initialize app configuration data
    */
    SAMPLE_AppData.PipeDepth = SAMPLE_PIPE_DEPTH;

    strcpy(SAMPLE_AppData.PipeName, "SAMPLE_CMD_PIPE");

    /*
    ** Initialize event filter table...
    */
    SAMPLE_AppData.EventFilters[0].EventID = SAMPLE_STARTUP_INF_EID;
    SAMPLE_AppData.EventFilters[0].Mask    = 0x0000;
    SAMPLE_AppData.EventFilters[1].EventID = SAMPLE_COMMAND_ERR_EID;
    SAMPLE_AppData.EventFilters[1].Mask    = 0x0000;
    SAMPLE_AppData.EventFilters[2].EventID = SAMPLE_COMMANDNOP_INF_EID;
    SAMPLE_AppData.EventFilters[2].Mask    = 0x0000;
    SAMPLE_AppData.EventFilters[3].EventID = SAMPLE_COMMANDRST_INF_EID;
    SAMPLE_AppData.EventFilters[3].Mask    = 0x0000;
    SAMPLE_AppData.EventFilters[4].EventID = SAMPLE_INVALID_MSGID_ERR_EID;
    SAMPLE_AppData.EventFilters[4].Mask    = 0x0000;
    SAMPLE_AppData.EventFilters[5].EventID = SAMPLE_LEN_ERR_EID;
    SAMPLE_AppData.EventFilters[5].Mask    = 0x0000;
    SAMPLE_AppData.EventFilters[6].EventID = SAMPLE_PIPE_ERR_EID;
    SAMPLE_AppData.EventFilters[6].Mask    = 0x0000;

    /*
    ** Register the events
    */
    status = CFE_EVS_Register(SAMPLE_AppData.EventFilters,
                              SAMPLE_EVENT_COUNTS,
                              CFE_EVS_EventFilter_BINARY);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Sample App: Error Registering Events, RC = 0x%08lX\n",
                             (unsigned long)status);
        return ( status );
    }

    /*
    ** Initialize housekeeping packet (clear user data area).
    */
    CFE_SB_InitMsg(&SAMPLE_AppData.HkBuf.MsgHdr,
                   SAMPLE_APP_HK_TLM_MID,
                   sizeof(SAMPLE_AppData.HkBuf),
                   true);

    /*
    ** Create Software Bus message pipe.
    */
    status = CFE_SB_CreatePipe(&SAMPLE_AppData.CommandPipe,
                               SAMPLE_AppData.PipeDepth,
                               SAMPLE_AppData.PipeName);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SAMPLE_APP_PIPE_CREATE_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "Sample App: Error creating pipe, RC = 0x%08lX\n",
                          (unsigned long)status);
        return ( status );
    }

    /*
    ** Subscribe to Housekeeping request commands
    */
    status = CFE_SB_Subscribe(SAMPLE_APP_SEND_HK_MID,
                              SAMPLE_AppData.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SAMPLE_APP_SUB_HK_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "Sample App: Error Subscribing to HK request, RC = 0x%08lX\n",
                          (unsigned long)status);
        return ( status );
    }

    /*
    ** Subscribe to ground command packets
    */
    status = CFE_SB_Subscribe(SAMPLE_APP_CMD_MID,
                              SAMPLE_AppData.CommandPipe);
    if (status != CFE_SUCCESS )
    {
        CFE_EVS_SendEvent(SAMPLE_APP_SUB_CMD_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "Sample App: Error Subscribing to Command, RC = 0x%08lX\n",
                          (unsigned long)status);

        return ( status );
    }

    /*
    ** Register Table(s)
    */
    status = CFE_TBL_Register(&SAMPLE_AppData.TblHandles[0],
                              "SampleTable",
                              sizeof(SAMPLE_Table_t),
                              CFE_TBL_OPT_DEFAULT,
                              SAMPLE_TblValidationFunc);
    if ( status != CFE_SUCCESS )
    {
        CFE_EVS_SendEvent(SAMPLE_APP_TBL_REG_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "Sample App: Error Registering Table, RC = 0x%08lX\n", 
                          (unsigned long)status);

        return ( status );
    }
    else
    {
        status = CFE_TBL_Load(SAMPLE_AppData.TblHandles[0],
                              CFE_TBL_SRC_FILE,
                              SAMPLE_TABLE_FILE);
    }

    CFE_EVS_SendEvent (SAMPLE_STARTUP_INF_EID,
                       CFE_EVS_EventType_INFORMATION,
                       "SAMPLE App Initialized. Version %d.%d.%d.%d",
                       SAMPLE_APP_MAJOR_VERSION,
                       SAMPLE_APP_MINOR_VERSION,
                       SAMPLE_APP_REVISION,
                       SAMPLE_APP_MISSION_REV);

    return ( CFE_SUCCESS );

} /* End of SAMPLE_AppInit() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  SAMPLE_ProcessCommandPacket                                        */
/*                                                                            */
/*  Purpose:                                                                  */
/*     This routine will process any packet that is received on the SAMPLE    */
/*     command pipe.                                                          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void SAMPLE_ProcessCommandPacket( CFE_SB_MsgPtr_t Msg )
{
    CFE_SB_MsgId_t  MsgId;

    MsgId = CFE_SB_GetMsgId(Msg);

    switch (MsgId)
    {
        case SAMPLE_APP_CMD_MID:
            SAMPLE_ProcessGroundCommand(Msg);
            break;

        case SAMPLE_APP_SEND_HK_MID:
            SAMPLE_ReportHousekeeping((CCSDS_CommandPacket_t *)Msg);
            break;

        default:
            CFE_EVS_SendEvent(SAMPLE_INVALID_MSGID_ERR_EID,
                              CFE_EVS_EventType_ERROR,
         	                    "SAMPLE: invalid command packet,MID = 0x%x",
                              (unsigned int)CFE_SB_MsgIdToValue(MsgId));
            break;
    }

    return;

} /* End SAMPLE_ProcessCommandPacket */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* SAMPLE_ProcessGroundCommand() -- SAMPLE ground commands                    */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void SAMPLE_ProcessGroundCommand( CFE_SB_MsgPtr_t Msg )
{
    uint16 CommandCode;
    CommandCode = CFE_SB_GetCmdCode(Msg);

    /*
    ** Process "known" SAMPLE app ground commands
    */
    switch (CommandCode)
    {
        case SAMPLE_APP_NOOP_CC:
            if(SAMPLE_VerifyCmdLength(Msg, sizeof(SAMPLE_Noop_t)))
            { 
                SAMPLE_Noop((SAMPLE_Noop_t *)Msg);
            }
            break;

        case SAMPLE_APP_RESET_COUNTERS_CC:
            if(SAMPLE_VerifyCmdLength(Msg, sizeof(SAMPLE_ResetCounters_t))) 
            { 
                SAMPLE_ResetCounters((SAMPLE_ResetCounters_t *)Msg);
            }
            break;

        case SAMPLE_APP_PROCESS_CC:
            if(SAMPLE_VerifyCmdLength(Msg, sizeof(SAMPLE_Process_t))) 
            { 
                SAMPLE_Process((SAMPLE_Process_t *)Msg);
            }
            break;

        /* default case already found during FC vs length test */
        default:
            CFE_EVS_SendEvent(SAMPLE_COMMAND_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "Invalid ground command code: CC = %d",
                              CommandCode);
            break;
    }

    return;

} /* End of SAMPLE_ProcessGroundCommand() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  SAMPLE_ReportHousekeeping                                          */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function is triggered in response to a task telemetry request */
/*         from the housekeeping task. This function will gather the Apps     */
/*         telemetry, packetize it and send it to the housekeeping task via   */
/*         the software bus                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 SAMPLE_ReportHousekeeping( const CCSDS_CommandPacket_t *Msg )
{
    int i;

    /*
    ** Get command execution counters...
    */
    SAMPLE_AppData.HkBuf.HkTlm.Payload.CommandErrorCounter = SAMPLE_AppData.ErrCounter;
    SAMPLE_AppData.HkBuf.HkTlm.Payload.CommandCounter = SAMPLE_AppData.CmdCounter;

    /*
    ** Send housekeeping telemetry packet...
    */
    CFE_SB_TimeStampMsg(&SAMPLE_AppData.HkBuf.MsgHdr);
    CFE_SB_SendMsg(&SAMPLE_AppData.HkBuf.MsgHdr);

    /*
    ** Manage any pending table loads, validations, etc.
    */
    for (i=0; i<SAMPLE_NUMBER_OF_TABLES; i++)
    {
        CFE_TBL_Manage(SAMPLE_AppData.TblHandles[i]);
    }

    return CFE_SUCCESS;

} /* End of SAMPLE_ReportHousekeeping() */



