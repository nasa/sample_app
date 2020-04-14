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
** File: sample_app_cmds.c
**
** Purpose:
**   This file contains the source code for the Sample App command handling
**   functions.
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


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* SAMPLE_Noop -- SAMPLE NOOP commands                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void SAMPLE_Noop( const SAMPLE_Noop_t *Msg )
{

    if (SAMPLE_VerifyCmdLength(Msg, sizeof(SAMPLE_Noop_t)))
    {
        SAMPLE_AppData.CmdCounter++;

        CFE_EVS_SendEvent(SAMPLE_COMMANDNOP_INF_EID,
                          CFE_EVS_EventType_INFORMATION,
                          "SAMPLE: NOOP command  Version %d.%d.%d.%d",
                          SAMPLE_APP_MAJOR_VERSION,
                          SAMPLE_APP_MINOR_VERSION,
                          SAMPLE_APP_REVISION,
                          SAMPLE_APP_MISSION_REV);
    }

    return CFE_SUCCESS;

} /* End of SAMPLE_Noop */

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

    if (SAMPLE_VerifyCmdLength(Msg, sizeof(SAMPLE_Noop_t)))
    {
        SAMPLE_AppData.CmdCounter = 0;
        SAMPLE_AppData.ErrCounter = 0;

        CFE_EVS_SendEvent(SAMPLE_COMMANDRST_INF_EID,
                          CFE_EVS_EventType_INFORMATION,
                          "SAMPLE: RESET command");
    }

    return CFE_SUCCESS;

} /* End of SAMPLE_ResetCounters() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  SAMPLE_Process                                                     */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function Process Ground Station Command                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void SAMPLE_Process( const SAMPLE_Process_t *Msg )
{
    int32 status;
    SAMPLE_Table_t *TblPtr;
    const char *TableName = "SAMPLE_APP.SampleTable";

    
    if (SAMPLE_VerifyCmdLength(Msg, sizeof(SAMPLE_Noop_t)))
    {
        /* Sample Use of Table */

        status = CFE_TBL_GetAddress((void *)&TblPtr,
                                    SAMPLE_AppData.TblHandles[0]);

        if (status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(SAMPLE_APP_TBL_ADDR_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "Sample App: Fail to get table address: 0x%08lx",
                              (unsigned long) status);
            
            SAMPLE_AppData.ErrCounter++;
            return;
        }

        CFE_EVS_SendEvent(SAMPLE_APP_TBL_INF_EID,
                          CFE_EVS_EventType_INFORMATION,
                          "Sample App: Table Value 1: %d  Value 2: %d",
                          TblPtr->Int1,
                          TblPtr->Int2);

        SAMPLE_GetCrc(TableName);

        status = CFE_TBL_ReleaseAddress(SAMPLE_AppData.TblHandles[0]);
        if (status != CFE_SUCCESS)
        {   
            CFE_EVS_SendEvent(SAMPLE_APP_TBL_REL_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "Sample App: Fail to release table address: 0x%08lx",
                              (unsigned long)status);
            
            SAMPLE_AppData.ErrCounter++;
            return;
        }

        /* Invoke a function provided by SAMPLE_LIB */
        SAMPLE_Function();
        
        SAMPLE_AppData.CmdCounter++;
    }

} /* End of SAMPLE_ProcessCC */


