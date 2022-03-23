/**
 * @file
 *
 *  The Sample App header file containing version information
 */

#ifndef SAMPLE_APP_VERSION_H
#define SAMPLE_APP_VERSION_H

/* Development Build Macro Definitions */

#define SAMPLE_APP_BUILD_NUMBER 9 /*!< Development Build: Number of commits since baseline */
#define SAMPLE_APP_BUILD_BASELINE \
    "v1.3.0-rc4" /*!< Development Build: git tag that is the base for the current development */

/* Version Macro Definitions */

#define SAMPLE_APP_MAJOR_VERSION 1 /*!< @brief ONLY APPLY for OFFICIAL releases. Major version number. */
#define SAMPLE_APP_MINOR_VERSION 1 /*!< @brief ONLY APPLY for OFFICIAL releases. Minor version number. */
#define SAMPLE_APP_REVISION      0 /*!< @brief ONLY APPLY for OFFICIAL releases. Revision version number. */

/*!
 * @brief Mission revision.
 *
 * Set to 0 on OFFICIAL releases, and set to 255 (0xFF) on development versions.
 * Values 1-254 are reserved for mission use to denote patches/customizations as needed.
 */
#define SAMPLE_APP_MISSION_REV 0xFF

#define SAMPLE_APP_STR_HELPER(x) #x /*!< @brief Helper function to concatenate strings from integer macros */
#define SAMPLE_APP_STR(x) \
    SAMPLE_APP_STR_HELPER(x) /*!< @brief Helper function to concatenate strings from integer macros */

/*! @brief Development Build Version Number.
 * @details Baseline git tag + Number of commits since baseline. @n
 * See @ref cfsversions for format differences between development and release versions.
 */
#define SAMPLE_APP_VERSION SAMPLE_APP_BUILD_BASELINE "+dev" SAMPLE_APP_STR(SAMPLE_APP_BUILD_NUMBER)

/*! @brief Development Build Version String.
 * @details Reports the current development build's baseline, number, and name. Also includes a note about the latest
 * official version. @n See @ref cfsversions for format differences between development and release versions.
 */
#define SAMPLE_APP_VERSION_STRING                       \
    " Sample App DEVELOPMENT BUILD " SAMPLE_APP_VERSION \
    ", Last Official Release: v1.1.0" /* For full support please use this version */

#endif /* SAMPLE_APP_VERSION_H */
