@echo off
chcp 65001 >nul
echo ============================================
echo   B2 Console Build
echo ============================================
echo.

call "D:\visual studio\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo ERROR: vcvars64.bat failed
    exit /b 1
)

cd /d "e:\PracticeWeek\B2"

echo Compiling B2 (4 source files)...
cl.exe /utf-8 /EHsc /Zi /std:c++17 /Fe:telecom_billing.exe callist.cpp charge.cpp user.cpp main.cpp
if %ERRORLEVEL% neq 0 (
    echo.
    echo ERROR: Compilation failed!
    exit /b 1
)

echo.
echo ============================================
echo   SUCCESS: telecom_billing.exe built!
echo ============================================
