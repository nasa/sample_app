# Core Flight System : Framework : App : Sample

This repository contains a sample application (sample_app), which is a framework component of the Core Flight System.

This sample application is a non-flight example application implementation for the cFS Bundle. It is intended to be located in the `apps/sample_app` subdirectory of a cFS Mission Tree.  The Core Flight System is bundled at https://github.com/nasa/cFS (which includes sample_app as a submodule), which includes build and execution instructions.

sample_app is an example for how to build and link an application in cFS.

## Version Notes
- 1.1.5  
  - Fix to build on RASPBIAN OS
  - Minor updates (see https://github.com/nasa/sample_app/pull/47)
- 1.1.4  
  - Fix for a clean build with OMIT_DEPRECATED
  - Minor updates (see https://github.com/nasa/sample_app/pull/44)
- 1.1.3
  - Minor updates (see https://github.com/nasa/sample_app/pull/34)
- 1.1.2
  - Minor updates (see https://github.com/nasa/sample_app/pull/20)
- 1.1.1
  - Minor updates (see https://github.com/nasa/sample_app/pull/15)
- **1.1.0 OFFICIAL RELEASE**:
  - Minor updates (see https://github.com/nasa/sample_app/pull/11)
  - Not backwards compatible with OSAL 4.2.1
  - Released as part of cFE 6.7.0, Apache 2.0
- **1.0.0a OFFICIAL RELEASE**:
  - Released as part of cFE 6.6.0a, Apache 2.0

## Known issues

As a sample application, extensive testing is not performed prior to release and only minimal functionality is included.  Note discrepancies likely exist between this application and the example detailed in the application developer guide.

## Getting Help

For best results, submit issues:questions or issues:help wanted requests at https://github.com/nasa/cFS.

Official cFS page: http://cfs.gsfc.nasa.gov
