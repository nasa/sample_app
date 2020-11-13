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
** File: sample_app_msg.h
**
** Purpose:
**  Define SAMPLE App  Messages and info
**
** Notes:
**
**
*******************************************************************************/
#ifndef _sample_app_msg_h_
#define _sample_app_msg_h_

/*
** SAMPLE App command codes
*/
#define SAMPLE_APP_NOOP_CC           0
#define SAMPLE_APP_RESET_COUNTERS_CC 1
#define SAMPLE_APP_PROCESS_CC        2

/*************************************************************************/

/*
** Type definition (generic "no arguments" command)
*/
typedef struct
{
    uint8 CmdHeader[CFE_SB_CMD_HDR_SIZE];

} SAMPLE_APP_NoArgsCmd_t;

/*
** The following commands all share the "NoArgs" format
**
** They are each given their own type name matching the command name, which_open_mode
** allows them to change independently in the future without changing the prototype
** of the handler function
*/
typedef SAMPLE_APP_NoArgsCmd_t SAMPLE_APP_Noop_t;
typedef SAMPLE_APP_NoArgsCmd_t SAMPLE_APP_ResetCounters_t;
typedef SAMPLE_APP_NoArgsCmd_t SAMPLE_APP_Process_t;

/*************************************************************************/
/*
** Type definition (SAMPLE App housekeeping)
*/

typedef struct
{
    uint8 CommandErrorCounter;
    uint8 CommandCounter;
    uint8 spare[2];
} SAMPLE_APP_HkTlm_Payload_t;

typedef struct
{
    CFE_SB_TlmHdr_t            TlmHeader;
    SAMPLE_APP_HkTlm_Payload_t Payload;
} SAMPLE_APP_HkTlm_t;


#endif /* _sample_app_msg_h_ */

/************************/
/*  End of File Comment */
/************************/
