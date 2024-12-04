@echo off

set OUTPUT_DIR=%1

set PYTHON_BIN=%2
if "%PYTHON_BIN%"=="" set PYTHON_BIN=python3

REM Create virtual environment
%PYTHON_BIN% -m venv .venv
call .venv\Scripts\activate.bat
pip install mypy build setuptools

cd %OUTPUT_DIR%

stubgen -m libbarpy -o .
cd ..
%PYTHON_BIN% -m build

@REM %PYTHON_BIN% setup.py bdist_wheel

@REM REM Test install
@REM pip install dist\raster_barcode-*.whl

