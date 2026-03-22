@echo off
setlocal EnableDelayedExpansion

REM ============================================
REM  Donut.cpp – Build & Run launcher
REM ============================================

REM Ensure bin\ exists
if not exist bin mkdir bin

REM --- Compile all sources --------------------
echo.
echo [BUILD] Compiling all sources...
echo.

set BUILD_OK=1

call :compile donut          src\donut.cpp
call :compile donut_improved src\donut_improved.cpp
call :compile cube           src\cube.cpp
call :compile pyramid        src\pyramid.cpp

if "!BUILD_OK!"=="0" (
    echo.
    echo [WARN] One or more files failed to compile.
)

REM --- Interactive menu loop ------------------
:MENU
echo.
echo ============================================
echo   Select a shape to run:
echo   1. Donut          (classic)
echo   2. Donut Improved (coloured)
echo   3. Cube
echo   4. Pyramid
echo   0. Exit
echo ============================================
set /p CHOICE="  Your choice: "

set TARGET=
if "%CHOICE%"=="1" set TARGET=donut
if "%CHOICE%"=="2" set TARGET=donut_improved
if "%CHOICE%"=="3" set TARGET=cube
if "%CHOICE%"=="4" set TARGET=pyramid
if "%CHOICE%"=="0" goto :EOF

if not defined TARGET (
    echo [ERROR] Invalid choice. Try again.
    goto MENU
)

if not exist "bin\%TARGET%.exe" (
    echo [ERROR] bin\%TARGET%.exe was not built. Check compiler errors above.
    goto MENU
)

echo.
echo [RUN] Running %TARGET%...  (press Ctrl+C to stop early)
echo.
"bin\%TARGET%.exe"

REM After the program exits, offer to run again or pick another
:AFTER_RUN
echo.
echo ============================================
echo   Done. What next?
echo   R. Run the same shape again
echo   M. Back to the menu
echo   0. Exit
echo ============================================
set /p NEXT="  Your choice: "

if /i "%NEXT%"=="R" goto :RUN_AGAIN
if /i "%NEXT%"=="M" goto MENU
if "%NEXT%"=="0" goto :EOF
goto AFTER_RUN

:RUN_AGAIN
echo.
echo [RUN] Running %TARGET% again...
echo.
"bin\%TARGET%.exe"
goto AFTER_RUN

REM ============================================
REM  Subroutine: compile <name> <source>
REM ============================================
:compile
set NAME=%~1
set SRC=%~2
echo   Compiling %SRC% ...
g++ -O2 -o "bin\%NAME%.exe" "%SRC%" -lm 2>&1
if errorlevel 1 (
    echo   [FAIL] %NAME%
    set BUILD_OK=0
) else (
    echo   [OK]   %NAME%
)
exit /b
