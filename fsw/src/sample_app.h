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
** File: sample_app.h
**
** Purpose:
**   This file is main hdr file for the SAMPLE application.
**
**
*******************************************************************************/

#ifndef _sample_app_h_
#define _sample_app_h_

/*
** Required header files.
*/
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

#include "sample_app_perfids.h"
#include "sample_app_msgids.h"
#include "sample_app_msg.h"

/***********************************************************************/
#define SAMPLE_PIPE_DEPTH                     32 /* Depth of the Command Pipe for Application */

#define NUMBER_OF_TABLES                      1  /* Number of Table(s) */

/* Define filenames of default data images for tables */
#define SAMPLE_TABLE_FILE                     "/cf/sample_table.tbl"

#define SAMPLE_TABLE_OUT_OF_RANGE_ERR_CODE    -1

#define SAMPLE_TBL_ELEMENT_1_MAX              10
/************************************************************************
** Type Definitions
*************************************************************************/
/*
** Global Data
*/
typedef struct
{
    /*
    ** Command interface counters...
    */
    uint8                 CmdCounter;
    uint8                 ErrCounter;

    /*
    ** Housekeeping telemetry packet...
    */
    sample_hk_tlm_t    SAMPLE_HkTelemetryPkt;

    /*
    ** Run Status variable used in the main processing loop
    */
    uint32 RunStatus;

    /*
    ** Operational data (not reported in housekeeping)...
    */
    CFE_SB_PipeId_t    SAMPLE_CommandPipe;
    CFE_SB_MsgPtr_t    SAMPLEMsgPtr;

    /*
    ** Initialization data (not reported in housekeeping)...
    */
    char     PipeName[16];
    uint16   PipeDepth;

    CFE_EVS_BinFilter_t  SAMPLE_EventFilters[SAMPLE_EVENT_COUNTS];
    CFE_TBL_Handle_t     TblHandles[NUMBER_OF_TABLES];

} Sample_AppData_t;

/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point (SAMPLE_AppMain), these
**       functions are not called from any other source module.
*/
void  SAMPLE_AppMain(void);
int32 SAMPLE_AppInit(void);
void  SAMPLE_ProcessCommandPacket(CFE_SB_MsgPtr_t Msg);
void  SAMPLE_ProcessGroundCommand(CFE_SB_MsgPtr_t Msg);
void  SAMPLE_ReportHousekeeping(const CCSDS_CommandPacket_t *Msg);
void  SAMPLE_ResetCounters(const SAMPLE_ResetCounters_t *Msg);
void  SAMPLE_ProcessCC(const SAMPLE_Process_t *Msg);
void  SAMPLE_NoopCmd(const SAMPLE_Noop_t *Msg);
void  SAMPLE_GetCrc(const char *TableName);

int32 SAMPLE_TblValidationFunc(void *TblData);

bool  SAMPLE_VerifyCmdLength(CFE_SB_MsgPtr_t Msg, uint16 ExpectedLength);

#endif /* _sample_app_h_ */
