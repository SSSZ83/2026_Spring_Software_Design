@echo off
chcp 65001 >nul
echo ============================================
echo   B2 Console Build
echo ============================================
echo.
echo [1/2] Setting up MSVC environment...
call "D:\visual studio\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo ERROR: vcvars64.bat failed
    exit /b 1
)
echo OK
echo [2/2] Compiling with cl.exe...
cl.exe /utf-8 /EHsc /Zi /std:c++17 /Fe:telecom_billing.exe callist.cpp charge.cpp user.cpp main.cpp
if %ERRORLEVEL% neq 0 (
    echo ERROR: compile failed
    exit /b 1
)
echo OK
echo.
echo ============================================
echo   SUCCESS: telecom_billing.exe built!
echo ============================================
