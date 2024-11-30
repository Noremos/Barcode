@echo off

set OUTPUT_DIR=%1

set PYTHON_BIN=%2
if "%PYTHON_BIN%"=="" set PYTHON_BIN=python3

REM Create virtual environment
%PYTHON_BIN% -m venv .venv
call .venv\Scripts\activate.bat
pip install mypy build

cd %OUTPUT_DIR%
stubgen -m libbarpy -o .
cd ..
%PYTHON_BIN% -m build

REM Test install
pip install dist\raster_barcode-*.whl

