@echo off
setlocal ENABLEDELAYEDEXPANSION
set FILE=%~dp0version.h
rem echo %FILE%
set MAJOR=3
set MINOR=4
set SUBVER=1
set BUILD=100
for /F "tokens=*" %%i in (%FILE%) do (
	rem echo %%i
        for /F "tokens=2,4,5,6 delims=. " %%k in ("%%i") do (
        	rem echo %%k - %%l
		if "%%k" == "VERSION" (
			set MINOR=%%l
			set SUBVER=%%m
			set BUILD=%%n
			if "%1" == "release" (
				set /A SUBVER=SUBVER+1 
			)
			set /A BUILD=BUILD+1 
		)
	)
)
echo #ifndef VERSION_H_ > %FILE%
echo #define VERSION_H_ >> %FILE%
echo #define VERSION "!MAJOR!.!MINOR!.!SUBVER!.!BUILD!" >> %FILE%
echo #endif // VERSION_H_ >> %FILE%
