@echo off
REM get installation directory of visual studio supplied by caller
set VS_INSTALLPATH=
call :PARSEARGLISTVALUE VS_INSTALLPATH -vspath %*
if "%VS_INSTALLPATH%" == "" (
	echo "No installation directory for Visual Studio given"
	exit /B 1
)
if not exist "%VS_INSTALLPATH%" (
	echo "Visual Studio installation directory does not exist"
	exit /B 1
)
set VCVARSPATH=
if not "%~1"=="-win32" set "VCVARSPATH=%VS_INSTALLPATH%\VC\bin\amd64"
if "%~1"=="-win32" set "VCVARSPATH=%VS_INSTALLPATH%\VC\bin"
if not exist "%VCVARSPATH%" if exist "%VS_INSTALLPATH%\VC\Auxiliary\Build" set "VCVARSPATH=%VS_INSTALLPATH%\VC\Auxiliary\Build"
if "%VCVARSPATH%" == "" (
	echo "path of vcvars.bat could not be found"
	exit /B 1
)
if not exist "%VCVARSPATH%" (
	echo "path of vcvars.bat does not exist"
	exit /B 1
)
set OLDPWD=%CD%
@echo on
REM set environment variables needed for nmake
if "%~1"=="-win32" call "%VCVARSPATH%\vcvars32.bat"
if not "%~1"=="-win32" call "%VCVARSPATH%\vcvars64.bat"
REM as of VS 2017 vcvars.bat changes the working directory
cd /D "%OLDPWD%"
REM call nmake
if exist makefilef2c.vc (
	nmake -f makefilef2c.vc
)
nmake -f makefile.vc
if "%ERRORLEVEL%" NEQ "0" goto :FAIL
exit /B 0

:PARSEARGLISTVALUE
	set ARGVAR=%~1
	set ARGSTRING=%~2
	shift
	shift
	:PARSEARGLISTVALUESTART
	if "%~1"=="" goto :PARSEARGLISTVALUEEND
	if "%~1"=="%ARGSTRING%" set "%ARGVAR%=%~2"
	if "%~1"=="%ARGSTRING%" goto :PARSEARGLISTVALUEEND
	shift
	goto :PARSEARGLISTVALUESTART
	:PARSEARGLISTVALUEEND
	exit /B 0
:FAIL
