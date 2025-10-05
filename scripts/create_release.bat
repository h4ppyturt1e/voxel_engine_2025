@echo off
REM Create a stable release of the voxel engine

set VERSION_FILE=VERSION
set BUILD_DIR=build
set RELEASE_DIR=stable_releases
set DEFAULT_CONFIG_DIR=default_config
set SRC_RELEASE_DIR=%BUILD_DIR%\bin\Release

REM Read version from VERSION file
for /f "delims=" %%i in (%VERSION_FILE%) do set VERSION=%%i

if "%VERSION%"=="" (
    echo Error: Could not read version from %VERSION_FILE%
    exit /b 1
)

echo Creating release version %VERSION%...

REM Create versioned directory
set RELEASE_PATH=%RELEASE_DIR%\voxel_engine_%VERSION%
if not exist "%RELEASE_PATH%" mkdir "%RELEASE_PATH%"

REM Copy entire compiled release folder (exe, config, assets, etc.)
if exist "%SRC_RELEASE_DIR%" (
    echo Copying files from %SRC_RELEASE_DIR% to %RELEASE_PATH% ...
    where robocopy >nul 2>&1
    if %ERRORLEVEL%==0 (
        robocopy "%SRC_RELEASE_DIR%" "%RELEASE_PATH%" /E >nul
    ) else (
        xcopy "%SRC_RELEASE_DIR%" "%RELEASE_PATH%" /E /I /Y >nul
    )
) else (
    echo Error: Source release directory not found: %SRC_RELEASE_DIR%
    exit /b 1
)

REM Include metadata files at release root
copy "VERSION" "%RELEASE_PATH%\VERSION" >nul
copy "README.md" "%RELEASE_PATH%\README.md" >nul

REM Copy default configs to release root (for first-run)
if exist "%DEFAULT_CONFIG_DIR%" (
    xcopy "%DEFAULT_CONFIG_DIR%" "%RELEASE_PATH%\default_config" /E /I /Y >nul
)

REM Ensure legacy path expected by ConfigManager exists: build\bin\Release\config
if exist "%RELEASE_PATH%\config" (
    if not exist "%RELEASE_PATH%\build\bin\Release\config" (
        mkdir "%RELEASE_PATH%\build\bin\Release\config" >nul 2>&1
    )
    xcopy "%RELEASE_PATH%\config" "%RELEASE_PATH%\build\bin\Release\config" /E /I /Y >nul
)

REM Create a simple run script
echo @echo off > "%RELEASE_PATH%\run.bat"
echo echo Starting Voxel Engine %VERSION%... >> "%RELEASE_PATH%\run.bat"
echo voxel_app.exe >> "%RELEASE_PATH%\run.bat"

echo Release created at: %RELEASE_PATH%
echo.
echo To run: cd "%RELEASE_PATH%" ^&^& run.bat
echo.
echo Version: %VERSION%
echo Executable: voxel_app.exe
echo Config: engine.ini
