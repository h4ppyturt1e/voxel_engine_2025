@echo off
REM Create a stable release of the voxel engine

set VERSION_FILE=VERSION
set BUILD_DIR=build
set RELEASE_DIR=stable_releases

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

REM Copy executable
copy "%BUILD_DIR%\bin\Release\voxel_app.exe" "%RELEASE_PATH%\voxel_app.exe"

REM Copy config files
copy "engine.ini" "%RELEASE_PATH%\engine.ini"
copy "VERSION" "%RELEASE_PATH%\VERSION"

REM Copy README
copy "README.md" "%RELEASE_PATH%\README.md"

REM Create a simple run script
echo @echo off > "%RELEASE_PATH%\run.bat"
echo echo Starting Voxel Engine %VERSION%... >> "%RELEASE_PATH%\run.bat"
echo voxel_app.exe >> "%RELEASE_PATH%\run.bat"

echo Release created at: %RELEASE_PATH%
echo.
echo To run: cd "%RELEASE_PATH%" && run.bat
echo.
echo Version: %VERSION%
echo Executable: voxel_app.exe
echo Config: engine.ini
