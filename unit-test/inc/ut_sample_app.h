/**
 * @file
 *
 *
 * Purpose:
 * Extra scaffolding functions for the sample_app unit test
 *
 * Notes:
 * This is an extra UT-specific extern declaration
 * to obtain access to an internal data structure
 *
 * UT often needs to modify internal data structures in ways that
 * actual applications never would (bypassing the normal API) in
 * order to exercise or set up for off-nominal cases.
 */

#ifndef UT_SAMPLE_APP_H
#define UT_SAMPLE_APP_H

/*
 * Necessary to include these here to get the definition of the
 * "SAMPLE_APP_Data_t" typedef.
 */
#include "sample_app_events.h"
#include "sample_app.h"

/*
 * Allow UT access to the global "SAMPLE_APP_Data" object.
 */
extern SAMPLE_APP_Data_t SAMPLE_APP_Data;

#endif /* UT_SAMPLE_APP_H */
