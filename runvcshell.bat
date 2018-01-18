rem use environment variables instead of Paths
set VECTOR_LICENSE_FILE=C:\SW-Tools\VCAST\FLEXlm\vector-FRN10710548.lic

set VCSHELLDIR=C:\Data\workdir\sandboxes\EPS_CORE_SW1_For_Tests\70_VectorCast\VCTest\vcshell


rem This shall be executed before the first Build to map the Y drive
rem C:
cd C:\Data\workdir\sandboxes\EPS_CORE_SW1_For_Tests\25_Build\Batch

call __INIT_PROJECT.bat

rem Delete the vcshell.db file


rem We shall be in the Y drive to execute 03_project_build.bat : how to go there ?
%VECTORCAST_DIR%\vcshell --echo --out=%VCSHELLDIR%\vcshell.txt --db=%VCSHELLDIR%\vcshell.db  03_project_build.bat