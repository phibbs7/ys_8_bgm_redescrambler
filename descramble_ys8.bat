@ECHO OFF
GOTO WORK
REM This script processes the directory given to it as a first argument looking for files to give to 
REM ys_8_bgm_redescrambler to descramble them. The output from test is then written to the 
REM directory indicated in the second argument or to the script's own directory.
REM
REM Ex Usage. descramble_ys8_bgm .\sound\stream\ .\output\
:WORK
IF DEFINED [%1]==[] GOTO ERROR

IF DEFINED [%2]==[] SET OUTPUT_DIR=%~dp0 && GOTO PROCESS
REM Otherwise we set OUTPUT_DIR to the user defined directory.
SET OUTPUT_DIR=%2
:PROCESS
ECHO Data output directory: %OUTPUT_DIR%
FOR /F "tokens=1 delims= " %%G IN ('DIR /B %1') DO ECHO "Processing file: %1%%G" && ys_8_bgm_redescrambler d %1%%G %OUTPUT_DIR%%%G.descramble.ogg 

GOTO END

:ERROR
ECHO "Usage: descramble_ys8_bgm <input directory> <OPTIONAL: output directory>"

:END
