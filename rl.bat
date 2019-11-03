@echo off
CLS

REM ----------------------------------------------------------------------------
REM MENU load or archive
REM ----------------------------------------------------------------------------

:start
  ECHO.
  ECHO what to do?                         [RapidLoader V1.05]
  ECHO -------------------------------------------------------------
  ECHO   [1] Load Device                   (overwriting!)
  ECHO   [2] Save Device                   (overwriting!)
  ECHO   [3] Save Device As...
  ECHO   [4] Copy new clean OmniESP.json   (overwriting!)
  ECHO   [5] Create NEW Device and Load
  ECHO   [6] Create NEW Module
  ECHO -------------------------------------------------------------
  ECHO   [7] Build bin-files and tarball
  ECHO   [8] Save Build to Archive         (overwriting!)
  ECHO -------------------------------------------------------------
  ECHO   [9] EXIT
  SET /P toDo=

  IF %toDo% == 1 GOTO loadDevice
  IF %toDo% == 2 GOTO safeDevice
  IF %toDo% == 3 GOTO safeAsDevice
  IF %toDo% == 4 GOTO copyOmniesp
  IF %toDo% == 5 GOTO newDevice
  IF %toDo% == 6 GOTO newModule
  IF %toDo% == 7 GOTO buildTarball
  IF %toDo% == 8 GOTO archiveBuild
  IF %toDo% == 9 GOTO end

  CLS
  ECHO Invalid Selection! Try again
  PAUSE
  CLS
  GOTO start

REM ----------------------------------------------------------------------------
REM load device from archive
REM ----------------------------------------------------------------------------

:loadDevice
  CLS
  ECHO load device into framework
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
  :readDir
    setlocal EnableDelayedExpansion
    CD _customDevices
    SET /a i = 0
    FOR /D %%G in (*) DO (
      SET /a i+=1
      SET "deviceDir[!i!]=%%~nxG"
      ECHO   [!i!] %%~nxG
    )
    SET /a i+=1
    ECHO   [!i!] EXIT
    CD ..
    ECHO.

    ECHO select Device.....

REM -----------------------------------------
REM choose device
REM -----------------------------------------

  SET /P dirCounter=
  IF %dirCounter% == !i! GOTO end

  CLS
  ECHO load Device: !deviceDir[%dirCounter%]!
  ECHO -------------------------------------------
  ECHO.

  REM -----------------------------------------
  REM copy Files
  REM -----------------------------------------

  ECHO copy files...
  XCOPY _customDevices\!deviceDir[%dirCounter%]!\data\customDevice\*.json data\customDevice\ /S /Y
  XCOPY _customDevices\!deviceDir[%dirCounter%]!\src\customDevice\customDevice*.* src\customDevice\ /S /Y
  XCOPY _customDevices\!deviceDir[%dirCounter%]!\firmware\version.json data /Y
  ECHO done
  ECHO.
  GOTO end

REM ----------------------------------------------------------------------------
REM save device to archive
REM ----------------------------------------------------------------------------

:safeDevice
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
  FOR /D %%G in (*) DO (
    SET /a i+=1
    SET "deviceDir[!i!]=%%~nxG"
    ECHO   [!i!] %%~nxG
  )
  SET /a i+=1
  ECHO   [!i!] EXIT
  CD ..
  ECHO.
  ECHO select Device.....

  REM -----------------------------------------
  REM choose device
  REM -----------------------------------------

  SET /P dirCounter=
  IF %dirCounter% == !i! GOTO end

  CLS
  ECHO archive Device: !deviceDir[%dirCounter%]!
  ECHO -------------------------------------------
  ECHO.

  REM -----------------------------------------
  REM clean archive
  REM -----------------------------------------
  ECHO clean archive files !deviceDir[%dirCounter%]!....
  DEL _customDevices\!deviceDir[%dirCounter%]!\data\customDevice\*.json
  DEL _customDevices\!deviceDir[%dirCounter%]!\src\customDevice\customDevice*.*
  ECHO done
  ECHO.

  REM -----------------------------------------
  REM copy Files
  REM -----------------------------------------

  ECHO archive files to !deviceDir[%dirCounter%]!
  XCOPY data\customDevice\*.json _customDevices\!deviceDir[%dirCounter%]!\data\customDevice\ /S /Y
  XCOPY src\customDevice\customDevice*.* _customDevices\!deviceDir[%dirCounter%]!\src\customDevice\ /S /Y
  ECHO done
  ECHO.
  GOTO end

REM ----------------------------------------------------------------------------
REM saveAS device to archive
REM ----------------------------------------------------------------------------

:safeAsDevice
  CLS
  ECHO Save Device as...
  ECHO -------------------------------------------
  ECHO.
  ECHO please enter name of new device.....
  SET /P deviceName=

  ECHO creating directorys for %deviceName%
  mkdir _customDevices\%deviceName%
  mkdir _customDevices\%deviceName%\src
  mkdir _customDevices\%deviceName%\data
  mkdir _customDevices\%deviceName%\firmware
  mkdir _customDevices\%deviceName%\doc
  mkdir _customDevices\%deviceName%\fhem

  REM -----------------------------------------
  REM copy Files
  REM -----------------------------------------

  ECHO archive files to %deviceName%
  XCOPY data\customDevice\*.json _customDevices\%deviceName%\data\customDevice\ /S /Y
  XCOPY src\customDevice\customDevice*.* _customDevices\%deviceName%\src\customDevice\ /S /Y
  ECHO done
  ECHO.
  GOTO end

REM ----------------------------------------------------------------------------
REM copy new clean OmniESP.json
REM ----------------------------------------------------------------------------

:copyOmniesp
  CLS
  ECHO copy OmniESP to data DIR
  ECHO -------------------------------------------
  ECHO.
  ECHO copy files...
  XCOPY _customDevices\OmniESP.json data /S /Y
  ECHO done
  ECHO.

REM ----------------------------------------------------------------------------
REM create NEW Device
REM ----------------------------------------------------------------------------

:newDevice
  CLS
  ECHO create NEW Device
  ECHO -------------------------------------------
  ECHO.
  ECHO please enter name of new device.....
  SET /P deviceName=

  ECHO creating directorys for %deviceName%
  mkdir _customDevices\%deviceName%
  mkdir _customDevices\%deviceName%\src
  mkdir _customDevices\%deviceName%\data
  mkdir _customDevices\%deviceName%\firmware
  mkdir _customDevices\%deviceName%\doc
  mkdir _customDevices\%deviceName%\fhem

  ECHO copy template to _customDevices\%deviceName%
  XCOPY _customDevices\_template\*.* _customDevices\%deviceName%\ /S /Y

  ECHO load %deviceName% into framework
  XCOPY _customDevices\%deviceName%\data\customDevice\*.json data\customDevice\ /S /Y
  XCOPY _customDevices\%deviceName%\src\customDevice\customDevice*.* src\customDevice\ /S /Y
  ECHO done

  ECHO.
  GOTO end

REM ----------------------------------------------------------------------------
REM create NEW Module
REM ----------------------------------------------------------------------------

:newModule
  CLS
  ECHO create NEW Module
  ECHO -------------------------------------------
  ECHO.
  ECHO please enter name of new module.....
  SET /P moduleName=

  ECHO creating files for %deviceName%
  XCOPY src\modules\_template\module.h src\modules\%moduleName%.h
  XCOPY src\modules\_template\module.cpp src\modules\%moduleName%.cpp

  ECHO.
  GOTO end

REM ----------------------------------------------------------------------------
REM build tarball
REM ----------------------------------------------------------------------------

:buildTarball
  CLS
  ECHO build bin-files and tarball
  ECHO -------------------------------------------
  ECHO.

  pio run -t clean
  pio run
  pio run -t buildfs
  gulp tarball

  ECHO.
  GOTO end

REM ----------------------------------------------------------------------------
REM archive build
REM ----------------------------------------------------------------------------

:archiveBuild
  CLS
  ECHO archive build
  ECHO -------------------------------------------

  REM -----------------------------------------
  REM read directory and show
  REM -----------------------------------------

  setlocal EnableDelayedExpansion
  CD _customDevices
  SET /a i = 0
  FOR /D %%G in (*) DO (
    SET /a i+=1
    SET "deviceDir[!i!]=%%~nxG"
    ECHO   [!i!] %%~nxG
  )
  SET /a i+=1
  ECHO   [!i!] EXIT
  CD ..
  ECHO.
  ECHO select Device.....

  REM -----------------------------------------
  REM choose device
  REM -----------------------------------------

  SET /P dirCounter=
  IF %dirCounter% == !i! GOTO end

  CLS
  ECHO archive build for Device: !deviceDir[%dirCounter%]!
  ECHO -------------------------------------------
  ECHO.

  REM -----------------------------------------
  REM save build
  REM -----------------------------------------

  XCOPY update\*.bin _customDevices\!deviceDir[%dirCounter%]!\firmware\ /S /Y
  XCOPY update\*.tar _customDevices\!deviceDir[%dirCounter%]!\firmware\ /S /Y
  XCOPY data\version.json _customDevices\!deviceDir[%dirCounter%]!\firmware\ /S /Y

  ECHO.
  GOTO end


:end
