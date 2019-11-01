/*******************************************************************************
**
**      GSC-18128-1, "Core Flight Executive Version 6.6"
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

#include <string.h>

/*
** global data
*/
Sample_AppData_t Sample_AppData;

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
    ** CFE_ES_APP_ERROR and the App will not enter the RunLoop
    */
    status = SAMPLE_AppInit();
    if (status != CFE_SUCCESS)
    {
        Sample_AppData.RunStatus = CFE_ES_APP_ERROR;
    }

    /*
    ** SAMPLE Runloop
    */
    while (CFE_ES_RunLoop(&Sample_AppData.RunStatus) == TRUE)
    {
        /*
        ** Performance Log Exit Stamp
        */
        CFE_ES_PerfLogExit(SAMPLE_APP_PERF_ID);

        /* Pend on receipt of command packet */
        status = CFE_SB_RcvMsg(&Sample_AppData.SAMPLEMsgPtr,
                               Sample_AppData.SAMPLE_CommandPipe,
                               CFE_SB_PEND_FOREVER);

        /*
        ** Performance Log Entry Stamp
        */
        CFE_ES_PerfLogEntry(SAMPLE_APP_PERF_ID);

        if (status == CFE_SUCCESS)
        {
            SAMPLE_ProcessCommandPacket(Sample_AppData.SAMPLEMsgPtr);
        }
        else
        {
            CFE_EVS_SendEvent(SAMPLE_PIPE_ERR_EID,
                              CFE_EVS_ERROR,
                              "SAMPLE APP: SB Pipe Read Error, App Will Exit");

            Sample_AppData.RunStatus = CFE_ES_APP_ERROR;
        }

    }

    /*
    ** Performance Log Exit Stamp
    */
    CFE_ES_PerfLogExit(SAMPLE_APP_PERF_ID);

    CFE_ES_ExitApp(Sample_AppData.RunStatus);

} /* End of SAMPLE_AppMain() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* SAMPLE_AppInit() --  initialization                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 SAMPLE_AppInit( void )
{
    int32    status;

    Sample_AppData.RunStatus = CFE_ES_APP_RUN;

    /*
    ** Initialize app command execution counters
    */
    Sample_AppData.CmdCounter = 0;
    Sample_AppData.ErrCounter = 0;

    /*
    ** Initialize app configuration data
    */
    Sample_AppData.PipeDepth = SAMPLE_PIPE_DEPTH;

    strcpy(Sample_AppData.PipeName, "SAMPLE_CMD_PIPE");

    /*
    ** Initialize event filter table...
    */
    Sample_AppData.SAMPLE_EventFilters[0].EventID = SAMPLE_STARTUP_INF_EID;
    Sample_AppData.SAMPLE_EventFilters[0].Mask    = 0x0000;
    Sample_AppData.SAMPLE_EventFilters[1].EventID = SAMPLE_COMMAND_ERR_EID;
    Sample_AppData.SAMPLE_EventFilters[1].Mask    = 0x0000;
    Sample_AppData.SAMPLE_EventFilters[2].EventID = SAMPLE_COMMANDNOP_INF_EID;
    Sample_AppData.SAMPLE_EventFilters[2].Mask    = 0x0000;
    Sample_AppData.SAMPLE_EventFilters[3].EventID = SAMPLE_COMMANDRST_INF_EID;
    Sample_AppData.SAMPLE_EventFilters[3].Mask    = 0x0000;
    Sample_AppData.SAMPLE_EventFilters[4].EventID = SAMPLE_INVALID_MSGID_ERR_EID;
    Sample_AppData.SAMPLE_EventFilters[4].Mask    = 0x0000;
    Sample_AppData.SAMPLE_EventFilters[5].EventID = SAMPLE_LEN_ERR_EID;
    Sample_AppData.SAMPLE_EventFilters[5].Mask    = 0x0000;
    Sample_AppData.SAMPLE_EventFilters[6].EventID = SAMPLE_PIPE_ERR_EID;
    Sample_AppData.SAMPLE_EventFilters[6].Mask    = 0x0000;

    /*
    ** Register the events
    */
    status = CFE_EVS_Register(Sample_AppData.SAMPLE_EventFilters,
                              SAMPLE_EVENT_COUNTS,
                              CFE_EVS_BINARY_FILTER);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Sample App: Error Registering Events, RC = 0x%08X\n",
                             status);
        return ( status );
    }

    /*
    ** Initialize housekeeping packet (clear user data area).
    */
    CFE_SB_InitMsg(&Sample_AppData.SAMPLE_HkTelemetryPkt,
                   SAMPLE_APP_HK_TLM_MID,
                   sizeof(sample_hk_tlm_t),
                   true);

    /*
    ** Create Software Bus message pipe.
    */
    status = CFE_SB_CreatePipe(&Sample_AppData.SAMPLE_CommandPipe,
                               Sample_AppData.PipeDepth,
                               Sample_AppData.PipeName);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Sample App: Error creating pipe, RC = 0x%08X\n",
                             status);
        return ( status );
    }

    /*
    ** Subscribe to Housekeeping request commands
    */
    status = CFE_SB_Subscribe(SAMPLE_APP_SEND_HK_MID,
                              Sample_AppData.SAMPLE_CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Sample App: Error Subscribing to HK request, RC = 0x%08X\n", 
                             status);
        return ( status );
    }

    /*
    ** Subscribe to ground command packets
    */
    status = CFE_SB_Subscribe(SAMPLE_APP_CMD_MID,
                              Sample_AppData.SAMPLE_CommandPipe);
    if (status != CFE_SUCCESS )
    {
        CFE_ES_WriteToSysLog("Sample App: Error Subscribing to Command, RC = 0x%08X\n",
                             status);

        return ( status );
    }

    /*
    ** Register Table(s)
    */
    status = CFE_TBL_Register(&Sample_AppData.TblHandles[0],
                              "SampleTable",
                              sizeof(SampleTable_t),
                              CFE_TBL_OPT_DEFAULT,
                              SAMPLE_TblValidationFunc);
    if ( status != CFE_SUCCESS )
    {
        CFE_ES_WriteToSysLog("Sample App: Error Registering \
                              Table, RC = 0x%08X\n", status);

        return ( status );
    }
    else
    {
        status = CFE_TBL_Load(Sample_AppData.TblHandles[0],
                              CFE_TBL_SRC_FILE,
                              SAMPLE_TABLE_FILE);
    }

    CFE_EVS_SendEvent (SAMPLE_STARTUP_INF_EID,
                       CFE_EVS_INFORMATION,
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
                              CFE_EVS_ERROR,
         	              "SAMPLE: invalid command packet,MID = 0x%x",
                              MsgId);
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
            if (SAMPLE_VerifyCmdLength(Msg, sizeof(SAMPLE_Noop_t)))
            {
                SAMPLE_NoopCmd((SAMPLE_Noop_t *)Msg);
            }

            break;

        case SAMPLE_APP_RESET_COUNTERS_CC:
            if (SAMPLE_VerifyCmdLength(Msg, sizeof(SAMPLE_ResetCounters_t)))
            {
                SAMPLE_ResetCounters((SAMPLE_ResetCounters_t *)Msg);
            }

            break;

        case SAMPLE_APP_PROCESS_CC:
            if (SAMPLE_VerifyCmdLength(Msg, sizeof(SAMPLE_Process_t)))
            {
                SAMPLE_ProcessCC((SAMPLE_Process_t *)Msg);
            }

            break;

        /* default case already found during FC vs length test */
        default:
            CFE_EVS_SendEvent(SAMPLE_COMMAND_ERR_EID,
                              CFE_EVS_ERROR,
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
void SAMPLE_ReportHousekeeping( const CCSDS_CommandPacket_t *Msg )
{
    int i;

    /*
    ** Get command execution counters...
    */
    Sample_AppData.SAMPLE_HkTelemetryPkt.sample_command_error_count = Sample_AppData.CmdCounter;
    Sample_AppData.SAMPLE_HkTelemetryPkt.sample_command_count = Sample_AppData.ErrCounter;

    /*
    ** Send housekeeping telemetry packet...
    */
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &Sample_AppData.SAMPLE_HkTelemetryPkt);
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &Sample_AppData.SAMPLE_HkTelemetryPkt);

    /*
    ** Manage any pending table loads, validations, etc.
    */
    for (i=0; i<NUMBER_OF_TABLES; i++)
    {
        CFE_TBL_Manage(Sample_AppData.TblHandles[i]);
    }

    return;

} /* End of SAMPLE_ReportHousekeeping() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* SAMPLE_NoopCmd -- SAMPLE NOOP commands                                     */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void SAMPLE_NoopCmd( const SAMPLE_Noop_t *Msg )
{

    Sample_AppData.CmdCounter++;

    CFE_EVS_SendEvent(SAMPLE_COMMANDNOP_INF_EID,
                      CFE_EVS_INFORMATION,
                      "SAMPLE: NOOP command  Version %d.%d.%d.%d",
                      SAMPLE_APP_MAJOR_VERSION,
                      SAMPLE_APP_MINOR_VERSION,
                      SAMPLE_APP_REVISION,
                      SAMPLE_APP_MISSION_REV);

    return;

} /* End of SAMPLE_NoopCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  SAMPLE_ResetCounters                                               */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry.                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void SAMPLE_ResetCounters( const SAMPLE_ResetCounters_t *Msg )
{

    Sample_AppData.CmdCounter = 0;
    Sample_AppData.ErrCounter = 0;

    CFE_EVS_SendEvent(SAMPLE_COMMANDRST_INF_EID,
                      CFE_EVS_INFORMATION,
                      "SAMPLE: RESET command");

    return;

} /* End of SAMPLE_ResetCounters() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  SAMPLE_ProcessCC                                                   */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function Process Ground Station Command                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void  SAMPLE_ProcessCC( const SAMPLE_Process_t *Msg )
{
    SampleTable_t *TblPtr;
    const char *TableName = "SAMPLE_APP.SampleTable";

    /* Sample Use of Table */
    CFE_TBL_GetAddress((void *)&TblPtr,
                        Sample_AppData.TblHandles[0]);

    CFE_ES_WriteToSysLog("Sample App: Table Value 1: %d  Value 2: %d",
                          TblPtr->Int1,
                          TblPtr->Int2);

    SAMPLE_GetCrc(TableName);

    return;

} /* End of SAMPLE_ProcessCC */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* SAMPLE_VerifyCmdLength() -- Verify command packet length                   */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
bool SAMPLE_VerifyCmdLength( CFE_SB_MsgPtr_t Msg, uint16 ExpectedLength )
{
    bool result = true;

    uint16 ActualLength = CFE_SB_GetTotalMsgLength(Msg);

    /*
    ** Verify the command packet length.
    */
    if (ExpectedLength != ActualLength)
    {
        CFE_SB_MsgId_t MessageID   = CFE_SB_GetMsgId(Msg);
        uint16         CommandCode = CFE_SB_GetCmdCode(Msg);

        CFE_EVS_SendEvent(SAMPLE_LEN_ERR_EID,
                          CFE_EVS_ERROR,
                          "Invalid Msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
                          MessageID,
                          CommandCode,
                          ActualLength,
                          ExpectedLength);

        result = false;

        Sample_AppData.ErrCounter++;
    }

    return( result );

} /* End of SAMPLE_VerifyCmdLength() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* SAMPLE_TblValidationFunc -- Verify contents of First Table      */
/* buffer contents                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 SAMPLE_TblValidationFunc( void *TblData )
{
    int32 ReturnCode = CFE_SUCCESS;
    SampleTable_t *TblDataPtr = (SampleTable_t *)TblData;

    /*
    ** Sample Table Validation
    */
    if (TblDataPtr->Int1 > SAMPLE_TBL_ELEMENT_1_MAX)
    {
        /* First element is out of range, return an appropriate error code */
        ReturnCode = SAMPLE_TABLE_OUT_OF_RANGE_ERR_CODE;
    }

    return ReturnCode;

} /* End of Sample_TblValidationFunc*/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* SAMPLE_GetCrc -- Output CRC                                     */
/*                                                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SAMPLE_GetCrc( const char *TableName )
{
    int32           status;
    uint32          Crc;
    CFE_TBL_Info_t  TblInfoPtr;

    status = CFE_TBL_GetInfo(&TblInfoPtr, TableName);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Sample App: Error Getting Table Info");
    }
    else
    {
        Crc = TblInfoPtr.Crc;
        CFE_ES_WriteToSysLog("Sample App: CRC: 0x%08X\n\n", Crc);
    }

    return;

} /* End of SAMPLE_GetCrc */
