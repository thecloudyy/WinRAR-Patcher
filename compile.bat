@echo off
setlocal
echo Compiling WinRAR-Crack.c to EXE...
echo.

REM Find compiler: prefer PATH, fall back to winget install location
where x86_64-w64-mingw32-gcc >nul 2>&1
if %errorlevel% equ 0 (
    set "CC=x86_64-w64-mingw32-gcc"
    set "WINDRES=x86_64-w64-mingw32-windres"
) else (
    set "CC="
    set "WINDRES="
    for /d %%D in ("%LOCALAPPDATA%\Microsoft\WinGet\Packages\MartinStorsjo.LLVM-MinGW.UCRT_*") do (
        for /d %%E in ("%%D\llvm-mingw-*") do (
            if exist "%%E\bin\x86_64-w64-mingw32-gcc.exe" set "CC=%%E\bin\x86_64-w64-mingw32-gcc.exe"
            if exist "%%E\bin\x86_64-w64-mingw32-windres.exe" set "WINDRES=%%E\bin\x86_64-w64-mingw32-windres.exe"
        )
    )
)

if not defined CC (
    echo Compiler x86_64-w64-mingw32-gcc not found.
    echo Install it with: winget install --id MartinStorsjo.LLVM-MinGW.UCRT --exact
    pause
    exit /b 1
)

if not exist "dist" mkdir dist

echo Using compiler: %CC%
"%WINDRES%" resource.rc -o resource.o
if %errorlevel% neq 0 (
    echo Resource compile FAILED!
    pause
    exit /b 1
)

REM Pure C + C driver: no libc++/libunwind dependency. -Os/-s keep it tiny.
"%CC%" -o "dist\WinRAR-Patcher.exe" "WinRAR-Patcher.c" resource.o -lshlwapi -luser32 -lshell32 -ladvapi32 -Os -s
set BUILDERR=%errorlevel%
del /q resource.o >nul 2>&1

if %BUILDERR% equ 0 (
    echo.
    echo ========================================
    echo   COMPILE SUCCESSFUL!
    echo ========================================
    echo.
    echo EXE location: dist\WinRAR-Patcher.exe
) else (
    echo.
    echo ========================================
    echo   COMPILE FAILED!
    echo ========================================
    echo.
)

pause
