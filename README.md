![Static Analysis](https://github.com/nasa/sample_app/workflows/Static%20Analysis/badge.svg)
![Format Check](https://github.com/nasa/sample_app/workflows/Format%20Check/badge.svg)

# Core Flight System : Framework : App : Sample

This repository contains a sample application (sample_app), which is a framework component of the Core Flight System.

This sample application is a non-flight example application implementation for the cFS Bundle. It is intended to be located in the `apps/sample_app` subdirectory of a cFS Mission Tree. The Core Flight System is bundled at <https://github.com/nasa/cFS> (which includes sample_app as a submodule), which includes build and execution instructions.

sample_app is an example for how to build and link an application in cFS. See also the skeleton_app (<https://github.com/nasa/skeleton_app>) if you are looking for a bare-bones application to which to add your business logic.

## Version History

### Development Build: v1.3.0-rc4+dev21

- Remove registration of empty event filters
- Update codeql workflow for reusable updates
- See <https://github.com/nasa/cFS/pull/505> 

### Development Build: v1.3.0-rc4+dev16

- Update copyright headers
- Standardize version information
- See <https://github.com/nasa/sample_app/pull/171> and <https://github.com/nasa/cFS/pull/445>
### Development Build: v1.3.0-rc4+dev9

- Use preferred UT patterns
- Reuse CodeQL, Static Analysis, and Format Check
- See <https://github.com/nasa/cFS/pull/414>

### Development Build: v1.3.0-rc4+dev4

- Use CFE_MSG_PTR conversion macro
- Update baseline for cFS-Caelum-rc4 to v1.3.0-rc4
- See <https://github.com/nasa/sample_app/pull/163> and <https://github.com/nasa/cFS/pull/390>

### Development Build: v1.2.0-rc1+dev73

- Apply CFE_SB_ValueToMsgId where required
- See <https://github.com/nasa/sample_app/pull/155> and <https://github.com/nasa/cFS/pull/359>
### Development Build: v1.2.0-rc1+dev66

-  Implement Coding Standard in CodeQL workflow
- See <https://github.com/nasa/sample_app/pull/150> and <https://github.com/nasa/cFS/pull/270>

### Development Build: v1.2.0-rc1+dev62

- Removes app registration call, `CFE_ES_RegisterApp()` since applications do not need to register themselves.
- Apply standard header guard on all header files by removing leading underscore. Convert file-scope block comments to doxygen format.
- See <https://github.com/nasa/sample_app/pull/145>


### Development Build: v1.2.0-rc1+dev56

- Replaces <> with " in local includes
- Adds CONTRIBUTING.md that links to the main cFS contributing guide.
- Adds a description for the requirements of command and telemetry Message IDs to explain why the Msg IDs have those requirements in documentation.
- See <https://github.com/nasa/sample_app/pull/137>

### Development Build: v1.2.0-rc1+dev48

- Simplify build to use wrappers and interface libs
- Add Testing Tools to the Security Policy
- See <https://github.com/nasa/sample_app/pull/130>

### Development Build: v1.2.0-rc1+dev37

- Documentation: Add `Security.md` with instructions on reporting vulnerabilities
- Resolves bug where success code was reported as an error for `CFE_TBL_GetAddress`.
- Rename `UT_ClearForceFail` as `UT_ClearDefaultValue` given change from <https://github.com/nasa/osal/issues/724>
- See <https://github.com/nasa/sample_app/pull/121>

### Development Build: v1.2.0-rc1+dev29

- Aligns messages according to changes in cFE <https://github.com/nasa/cFE/issues/1009>. Uses the "raw" message cmd/tlm types in definition
- See <https://github.com/nasa/sample_app/pull/114>


### Development Build: v1.2.0-rc1+dev25

- Rename `UT_SetForceFail` to `UT_SetDefaultReturnValue` since some functions that retain more than 1 value are not necessarily failing
- See <https://github.com/nasa/sample_app/pull/113>


### Development Build: v1.2.0-rc1+dev22

- Replaces deprecated SB API's with MSG
- No impact, removes undesirable pattern use of `OS_PACK`
- See <https://github.com/nasa/sample_app/pull/108>

### Development Build: v1.2.0-rc1+dev18

- No behavior changes. All identifiers now use the prefix `SAMPLE_APP_`. Changes the name of the main function from SAMPLE_AppMain to SAMPLE_APP_Main which affects the CFE startup script.
- Set REVISION to "99" to indicate development version status
- See <https://github.com/nasa/sample_app/pull/102>

### Development Build: v1.2.0-rc1+dev13

- Unit test MID string format now 32bit
- Installs unit test to target directory
- Checks only format string in UT event test
- See <https://github.com/nasa/sample_app/pull/98>

### Development Build: v1.2.0-rc1+dev5

- Applies standard coding style.
- Removes test code call of CFE_SB_InitMsg and sets the API/stub buffers directly.
- See <https://github.com/nasa/sample_app/pull/93>

### Development Build: 1.1.0+dev65

- Add build number and baseline to version report
- Install unit test as part of cmake recipe. Sample app test runner now shows up in expected install directory
- See <https://github.com/nasa/sample_app/pull/86>

### Development Build: 1.1.11

- Move the table to fsw/tables and renames "sample_table" to "sample_app_table
- See <https://github.com/nasa/sample_app/pull/76>

### Development Build: 1.1.10

- Test cases now compare an expected event string with a string derived from the spec string and arguments that were output by the unit under test.
- Replace references to `ccsds.h` types with the `cfe_sb.h`-provided type.
- See <https://github.com/nasa/sample_app/pull/71>

### Development Build: 1.1.9

- Applies the CFE_SB_MsgIdToValue() and CFE_SB_ValueToMsgId() routines where compatibility with an integer MsgId is necessary - syslog prints, events, compile-time MID #define values.
- No more format conversion error in RTEMS build
- See <https://github.com/nasa/sample_app/pull/63>

### Development Build: 1.1.8

- Coverage data from make lcov includes the sample_app code
- See <https://github.com/nasa/sample_app/pull/62>

### Development Build: 1.1.7

- Fix bug where table is not released after being used
- Minor updates (see <https://github.com/nasa/sample_app/pull/52>)

### Development Build: 1.1.6

- Minor updates (see <https://github.com/nasa/sample_app/pull/49>)

### Development Build: 1.1.5

- Fix to build on RASPBIAN OS
- Minor updates (see <https://github.com/nasa/sample_app/pull/47>)

### Development Build: 1.1.4

- Fix for a clean build with OMIT_DEPRECATED
- Minor updates (see <https://github.com/nasa/sample_app/pull/44>)

### Development Build: 1.1.3

- Minor updates (see <https://github.com/nasa/sample_app/pull/34>)

### Development Build: 1.1.2

- Minor updates (see <https://github.com/nasa/sample_app/pull/20>)

### Development Build: 1.1.1

- Minor updates (see <https://github.com/nasa/sample_app/pull/15>)

### _**OFFICIAL RELEASE: 1.1.0 - Aquila**_

- Minor updates (see <https://github.com/nasa/sample_app/pull/11>)
- Not backwards compatible with OSAL 4.2.1
- Released as part of cFE 6.7.0, Apache 2.0

### _**OFFICIAL RELEASE: 1.0.0a**_

- Released as part of cFE 6.6.0a, Apache 2.0

## Known issues

As a sample application, extensive testing is not performed prior to release and only minimal functionality is included. Note discrepancies likely exist between this application and the example detailed in the application developer guide.

## Getting Help

For best results, submit issues:questions or issues:help wanted requests at <https://github.com/nasa/cFS>.

Official cFS page: <http://cfs.gsfc.nasa.gov>
