# Core Flight System : Framework : App : Sample

This repository contains a sample application (sample_app), which is a framework component of the Core Flight System.

This sample application is a non-flight example application implementation for the cFS Bundle. It is intended to be located in the `apps/sample_app` subdirectory of a cFS Mission Tree.  The Core Flight System is bundled at https://github.com/nasa/cFS (which includes sample_app as a submodule), which includes build and execution instructions.

sample_app is an example for how to build and link an application in cFS.

## Release Notes

sample_app version 1.0.0a is released as part of cFE 6.6.0a under the Apache 2.0 license, see [LICENSE](LICENSE-18128-Apache-2_0.pdf).

Note the old GSFC Build toolset is deprecated (fsw/for_build/Makefile) in favor of cmake (CMakeLists.txt)

## Known issues

As a sample application, extensive testing is not performed prior to release and only minimal functionality is included.  Note discrepancies likely exist between this application and the example detailed in the application developer guide.

## Getting Help

The cFS community page http://coreflightsystem.org should be your first stop for getting help. Please post questions to http://coreflightsystem.org/questions/. There is also a forum at http://coreflightsystem.org/forums/ for more general discussions.

Official cFS page: http://cfs.gsfc.nasa.gov

