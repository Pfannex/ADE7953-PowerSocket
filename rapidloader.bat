@echo off
CLS

REM ----------------------------------------------------------------------------
REM MENU load or archive
REM ----------------------------------------------------------------------------

:start
ECHO.
ECHO what to do?
ECHO -------------------------------------------
ECHO.
ECHO [1] load device from archive
ECHO [2] archive modified device
ECHO [3] copy new clean OmniESP.json
ECHO [4] EXIT
SET /P toDo=

IF %toDo% == 1 GOTO loadDevice
IF %toDo% == 2 GOTO archiveDevice
IF %toDo% == 3 GOTO copyOmniesp
IF %toDo% == 4 GOTO end

CLS
ECHO Invalid Selection! Try again
PAUSE
CLS
GOTO start


REM ----------------------------------------------------------------------------
:loadDevice
CLS
ECHO load device from framework
ECHO -------------------------------------------
ECHO.
REM -----------------------------------------
REM clean framework
REM -----------------------------------------
ECHO clean framework files....
DEL data\customDevice\*.json
DEL src\customDevice\customDevice*.*
ECHO done
ECHO.

REM -----------------------------------------
REM read directory and show
REM -----------------------------------------

setlocal EnableDelayedExpansion
CD _customDevices
SET /a i = 0
FOR /D %%G in ("*") DO (
  SET /a i+=1
  SET "deviceDir[!i!]=%%~nxG"
  ECHO [!i!] %%~nxG
)
CD ..
ECHO.
ECHO select Device.....

REM -----------------------------------------
REM choose device
REM -----------------------------------------

SET /P dirCounter=
CLS
ECHO load Device: !deviceDir[%dirCounter%]!
ECHO -------------------------------------------
ECHO.

REM -----------------------------------------
REM copy Files
REM -----------------------------------------

ECHO copy files...
copy %~d0%~p0_customDevices\!deviceDir[%dirCounter%]!\data\customDevice\*.json data\customDevice
copy %~d0%~p0_customDevices\!deviceDir[%dirCounter%]!\src\customDevice\customDevice*.* src\customDevice
ECHO done

GOTO end

REM ----------------------------------------------------------------------------
:archiveDevice
CLS
ECHO archive Device
ECHO -------------------------------------------
ECHO.

REM -----------------------------------------
REM read directory and show
REM -----------------------------------------

setlocal EnableDelayedExpansion
CD _customDevices
SET /a i = 0
FOR /D %%G in ("*") DO (
  SET /a i+=1
  SET "deviceDir[!i!]=%%~nxG"
  ECHO [!i!] %%~nxG
)
CD ..
ECHO.
ECHO select Device.....

REM -----------------------------------------
REM choose device
REM -----------------------------------------

SET /P dirCounter=
CLS
ECHO archive Device: !deviceDir[%dirCounter%]!
ECHO -------------------------------------------
ECHO.

REM -----------------------------------------
REM clean archive
REM -----------------------------------------
ECHO clean archive files !deviceDir[%dirCounter%]!....
DEL _customDevices\!deviceDir[%dirCounter%]!\data\customDevice\*.json
DEL _customDevices\!deviceDir[%dirCounter%]!\firmware\*.bin
DEL _customDevices\!deviceDir[%dirCounter%]!\firmware\*.tar
DEL _customDevices\!deviceDir[%dirCounter%]!\src\customDevice\customDevice*.*
ECHO done
ECHO.

REM -----------------------------------------
REM copy Files
REM -----------------------------------------

ECHO archive files to !deviceDir[%dirCounter%]!
copy %~d0%~p0data\customDevice\*.json _customDevices\!deviceDir[%dirCounter%]!\data\customDevice
copy %~d0%~p0update\*.* _customDevices\!deviceDir[%dirCounter%]!\firmware
copy %~d0%~p0src\customDevice\customDevice*.* _customDevices\!deviceDir[%dirCounter%]!\src\customDevice
ECHO done

GOTO end

REM ----------------------------------------------------------------------------
:copyOmniesp
CLS
ECHO copy OmniESP to data DIR
ECHO -------------------------------------------
ECHO.
ECHO copy files...
copy %~d0%~p0_customDevices\OmniESP.json data

:end
