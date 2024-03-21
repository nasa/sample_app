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
 * @file
 *   This file contains the prototypes for the Sample App utility functions
 */

#ifndef SAMPLE_APP_UTILS_H
#define SAMPLE_APP_UTILS_H

/*
** Required header files.
*/
#include "sample_app.h"

CFE_Status_t SAMPLE_APP_TblValidationFunc(void *TblData);
void         SAMPLE_APP_GetCrc(const char *TableName);

#endif /* SAMPLE_APP_UTILS_H */