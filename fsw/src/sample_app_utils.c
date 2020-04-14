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
** File: sample_app_utils.c
**
** Purpose:
**   This file contains the source code for the Sample App utility functions.
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
                          CFE_EVS_EventType_ERROR,
                          "Invalid Msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
                          MessageID,
                          CommandCode,
                          ActualLength,
                          ExpectedLength);

        result = false;

        SAMPLE_AppData.ErrCounter++;
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
    SAMPLE_Table_t *TblDataPtr = (SAMPLE_Table_t *)TblData;

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
        CFE_EVS_SendEvent(SAMPLE_APP_TBL_GETINF_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "Sample App: Error Getting Table Info");
    }
    else
    {
        Crc = TblInfoPtr.Crc;
        CFE_EVS_SendEvent(SAMPLE_APP_TBL_CRC_INF_EID,
                          CFE_EVS_EventType_INFORMATION,
                          "Sample App: CRC: 0x%08lX\n\n", 
                          (unsigned long)Crc);
    }

    return;

} /* End of SAMPLE_GetCrc */
