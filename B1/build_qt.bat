@echo off
chcp 65001 >nul
echo ============================================
echo   B1 Qt GUI Build
echo ============================================
echo.

echo [1/3] Setting up MSVC environment...
call "D:\visual studio\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo ERROR: vcvars64.bat failed
    exit /b 1
)
echo OK

echo [2/3] Running qmake...
"D:\Anaconda\Library\bin\qmake.exe" B1_qt.pro -spec win32-msvc
if %ERRORLEVEL% neq 0 (
    echo ERROR: qmake failed
    exit /b 1
)
echo OK

echo [3/3] Compiling with nmake...
nmake
if %ERRORLEVEL% neq 0 (
    echo ERROR: nmake build failed
    exit /b 1
)
echo OK

echo [4/4] Copying qt.conf to release...
copy /Y qt.conf release\qt.conf >nul 2>&1
echo OK

echo.
echo ============================================
echo   SUCCESS: grade_system_qt.exe built!
echo   Run: release\grade_system_qt.exe
echo ============================================
