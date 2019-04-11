@echo off
REM 取当前系统时间,可能因操作系统不同而取值不一样  
set CURDATE=%date:~0,4%%date:~5,2%%date:~8,2%
::echo %CURDATE%
set CURMON=%date:~0,4%%date:~5,2%
::echo %CURMON%
set CURTIME=%time:~0,2%
REM 小时数如果小于10，则在前面补0  
if "%CURTIME%"==" 0" set CURTIME=00
if "%CURTIME%"==" 1" set CURTIME=01
if "%CURTIME%"==" 2" set CURTIME=02
if "%CURTIME%"==" 3" set CURTIME=03
if "%CURTIME%"==" 4" set CURTIME=04
if "%CURTIME%"==" 5" set CURTIME=05
if "%CURTIME%"==" 6" set CURTIME=06
if "%CURTIME%"==" 7" set CURTIME=07
if "%CURTIME%"==" 8" set CURTIME=08
if "%CURTIME%"==" 9" set CURTIME=09
set CURTIME=%CURTIME%%time:~3,2%%time:~6,2%
::echo %CURTIME%
set NAME=_%CURDATE%_%CURTIME%
::echo %NAME%
copy index.html bak\index%NAME%.html
copy .\css\style.css .\bak\css\style%NAME%.css
copy .\blog\index.html .\bak\blog\index%NAME%.html
pause