:: ---------------------------------------------------------------------------
:: Set if you want to autput the commands or only commands output
:: NOTE! if you set set ECHOOFF to FALSE all the comands will be printed
:: The variable can be set from other batch files calling this file 
:: ---------------------------------------------------------------------------
::@SET ECHOOFF=FALSE

@set BUILD_SERVER=FALSE
::If ECHOOFF is set to TRUE "echo is off", else is "on" 
@IF (%ECHOOFF%)==(FALSE) goto echoon
@echo off
:echoon
:: ---------------------------------------------------------------------------
:: Jump to the path in which this batch is placed
:: this is needed for calls like this:
:: C:\>y:\25_Build\Batch\00_DEINIT_project.bat
:: ---------------------------------------------------------------------------
PUSHD %~dp0

:: ---------------------------------------------------------------------------
:: Call __SET_ENV.bat file - contains the setting of the needed variables  
:: ---------------------------------------------------------------------------
@call __SET_ENV.bat 
:: ---------------------------------------------------------------------------
::Check to see if we were called from the right place
:: ---------------------------------------------------------------------------
call callerDriveCheck.bat

IF NOT (%CLEAR_SCREEN%)==(FALSE) cls

call GetTime
set duration=%ActTime%

:: Clear main.o to force compilation of main.c to update Software date and compile time
IF EXIST %PRJ_DRIVE%:\%BUILD_DIR%\Results\Objects\main.o del %PRJ_DRIVE%:\%BUILD_DIR%\Results\Objects\main.o

"%BIN_CYGWIN%/make" -C../Config/Make -f Makefile_Project.mk BUILD_TARGET=BLD_OBJ --no-print-directory 2>&1 | "%BIN_CYGWIN%/tee" "%LOG_FILE_NAME%"

call GetTime
set /a duration=%ActTime%-%duration%

SET /a hour=%duration%/3600 >nul
SET /a minute=(%duration%-%hour%*3600)/60 >nul
SET /a second=%duration%-%hour%*3600-%minute%*60 >nul
echo.
echo Compiling time: %minute%min %second%sec 


echo ===============================================================================
echo -                          . . . D O N E                                      -
echo ===============================================================================

pause

POPD
