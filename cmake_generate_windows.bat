#echo on

set CMAKE_COMMON_PARAMS=-DTARGET_BUILD_PLATFORM=windows

cmake -S . -B build %CMAKE_COMMON_PARAMS%

pause