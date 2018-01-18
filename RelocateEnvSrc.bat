set VCAST_CD=%cd%
set VCAST_ENV_NAME=VectorCAST_Coverage

set VCAST_ENV_SRC=C:\data\workdir\sandboxes\Sandbox_RebaseOut\70_VectorCast\Work\vcast-workarea\vc_coverage
set VCAST_CODE_SRC=Y:\

set VCAST_ENV_DST=C:\data\workdir\sandboxes\Sandbox_RebaseOut\70_VectorCast\Work\vcast-workarea\vc_coverage
set VCAST_CODE_DST=C:\data\workdir\sandboxes\Sandbox_RebaseOut
if exist %VCAST_ENV_DST%\%VCAST_ENV_NAME%_new.bat goto alreadyrun
cd /d %VCAST_ENV_SRC%
%VECTORCAST_DIR%\clicast -lc -e%VCAST_ENV_NAME% Cover Environment Regression_Script
if errorlevel 1 goto end
rem Copy the regression files
if "%VCAST_ENV_SRC%" == "%VCAST_ENV_DST%" goto nocopyenv
copy %VCAST_ENV_NAME%.* %VCAST_ENV_DST%
cd /d %VCAST_ENV_DST%
:nocopyenv
rem -Brian Leach- Next line is copy source files for my testing
rem copy %VCAST_CODE_SRC%\*.c %VCAST_CODE_DST%

%VECTORCAST_DIR%\vpython %VCAST_CD%\RelocatePathInScript.py %VCAST_ENV_NAME%.bat %VCAST_ENV_NAME%_new.bat %VCAST_ENV_SRC% %VCAST_ENV_DST% -s %VCAST_CODE_SRC% -d %VCAST_CODE_DST%
rem pause
if errorlevel 1 goto end
if not exist %VCAST_ENV_NAME%.vcp goto envnotexist
del %VCAST_ENV_NAME%.vcp
rd /s /q %VCAST_ENV_NAME%
:envnotexist
%VCAST_ENV_NAME%_new.bat
goto end
:alreadyrun
echo This batch file has already been run on this environment
:end
pause
