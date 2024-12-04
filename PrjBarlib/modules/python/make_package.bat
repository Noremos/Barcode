@echo off

set scriptpath=%~dp0

set BUILD_DIR=%1
set PYTHON_BIN=%2
set OUTPUT_DIR=%BUILD_DIR%\BarcodeProject\raster_barcode

if "%PYTHON_BIN%"=="" set PYTHON_BIN=python3

xcopy /Q /E /Y /I %scriptpath%\BarcodeProject %BUILD_DIR%\BarcodeProject
copy /B /Y "%BUILD_DIR%\libbarpy.pyd" "%OUTPUT_DIR%\libbarpy.pyd"

REM Create virtual environment
%PYTHON_BIN% -m venv .venv
call .venv\Scripts\activate.bat
pip install mypy build setuptools

cd %OUTPUT_DIR%

stubgen -m libbarpy -o .
%PYTHON_BIN% "%scriptpath%/correct_types.py" "libbarpy.pyi"

cd ..
%PYTHON_BIN% -m build

@REM %PYTHON_BIN% setup.py bdist_wheel

@REM REM Test install
@REM pip install dist\raster_barcode-*.whl

