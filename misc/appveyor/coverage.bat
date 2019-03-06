@ECHO OFF
REM This script cooperates with library.ps1 and behaves like the Valgrind.
REM Instead of mem-checking, it calls OpenCppCoverage.

REM Assumption: This file is called "coverage.bat".
REM Assumption: This file is inside "misc\appveyor" subdir.

REM This script should handle spaces in paths correctly.



REM Extract absolute path to the source directory.
SET "SOURCES=%~0"
REM Linux path separator is used by ctest - replace it.
SET "SOURCES=%SOURCES:/=\%"
IF NOT "%SOURCES:~-27%"=="\misc\appveyor\coverage.bat" EXIT /B 101
SET "SOURCES=%SOURCES:~0,-27%"

REM The first argument should be --log-file=PATH_TO_LOG_FILE
SET "LOG=%~1"
SHIFT
REM Sometimes, the = sign is missing and we have two arguments
IF "%LOG%"=="--log-file" (
    SET "LOG=%~1"
    SHIFT
) ELSE (
    REM Fail fast if arguments are strange.
    IF NOT "%LOG:~0,11%"=="--log-file=" EXIT /B 102
    SET "LOG=%LOG:~11%"
)


REM There should be "--@" argument to
REM delimit actual test suite command.
SET "TOKEN=%~1"
IF NOT "%TOKEN%"=="--@" IF NOT "%TOKEN:~1,-1%"=="--@" EXIT /B 103
SHIFT


REM Remaining arguments are the actual
REM test binary with all its arguments.
IF "%~1"=="" GOTO ARGS_CONT
SET "ARGS=%1"
SHIFT
:ARGS_LOOP
IF "%~1"=="" GOTO ARGS_CONT
SET "ARGS=%ARGS% %1"
SHIFT
GOTO ARGS_LOOP
:ARGS_CONT


REM Run the actual test command
OpenCppCoverage --export_type "binary:%LOG%.bin" --quiet --sources "%SOURCES%" --cover_children --excluded_line_regex "^ *\} *else *\{ *(//.*)?$" --excluded_line_regex "^ *[\{\}] *(//.*)?$" -- %ARGS% > "%LOG%"
EXIT /B
