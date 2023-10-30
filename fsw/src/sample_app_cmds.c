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
 *   This file contains the source code for the Sample App Ground Command-handling functions
 */

/*
** Include Files:
*/
#include "sample_app.h"
#include "sample_app_cmds.h"
#include "sample_app_eventids.h"
#include "sample_app_version.h"
#include "sample_app_tbl.h"
#include "sample_app_utils.h"
#include "sample_app_msg.h"

/* The sample_lib module provides the SAMPLE_Function() prototype */
#include "sample_lib.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* SAMPLE NOOP commands                                                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 SAMPLE_APP_Noop(const SAMPLE_APP_NoopCmd_t *Msg)
{
    SAMPLE_APP_Data.CmdCounter++;

    CFE_EVS_SendEvent(SAMPLE_APP_COMMANDNOP_INF_EID, CFE_EVS_EventType_INFORMATION, "SAMPLE: NOOP command %s",
                      SAMPLE_APP_VERSION);

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry.                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 SAMPLE_APP_ResetCounters(const SAMPLE_APP_ResetCountersCmd_t *Msg)
{
    SAMPLE_APP_Data.CmdCounter = 0;
    SAMPLE_APP_Data.ErrCounter = 0;

    CFE_EVS_SendEvent(SAMPLE_APP_COMMANDRST_INF_EID, CFE_EVS_EventType_INFORMATION, "SAMPLE: RESET command");

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function Process Ground Station Command                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 SAMPLE_APP_Process(const SAMPLE_APP_ProcessCmd_t *Msg)
{
    int32               status;
    SAMPLE_APP_Table_t *TblPtr;
    const char *        TableName = "SAMPLE_APP.SampleAppTable";

    /* Sample Use of Table */

    status = CFE_TBL_GetAddress((void *)&TblPtr, SAMPLE_APP_Data.TblHandles[0]);

    if (status < CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Sample App: Fail to get table address: 0x%08lx", (unsigned long)status);
        return status;
    }

    CFE_ES_WriteToSysLog("Sample App: Table Value 1: %d  Value 2: %d", TblPtr->Int1, TblPtr->Int2);

    SAMPLE_APP_GetCrc(TableName);

    status = CFE_TBL_ReleaseAddress(SAMPLE_APP_Data.TblHandles[0]);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Sample App: Fail to release table address: 0x%08lx", (unsigned long)status);
        return status;
    }

    /* Invoke a function provided by SAMPLE_APP_LIB */
    SAMPLE_LIB_Function();

    return CFE_SUCCESS;
}
