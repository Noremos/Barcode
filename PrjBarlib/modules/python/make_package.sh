#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
BUILD_DIR="$1"
OUTPUT_DIR="$1/BarcodeProject/raster_barcode"

PYTHON_BIN="$2"
if [ -z "$PYTHON_BIN" ]; then
	PYTHON_BIN="python3"
fi

cp -rf "$SCRIPT_DIR/BarcodeProject" "$BUILD_DIR/BarcodeProject"
cp "$BUILD_DIR/libbarpy.so" "$OUTPUT_DIR/"

cd "$OUTPUT_DIR"

# Create virtual environment
$PYTHON_BIN -m venv .venv
source ./.venv/bin/activate
pip install mypy build setuptools

stubgen -m libbarpy -o .

$PYTHON_BIN "$SCRIPT_DIR/correct_types.py" "libbarpy.pyi"

cd ..
$PYTHON_BIN -m build
# %PYTHON_BIN% setup.py bdist_wheel


# Test install
# pip install dist/raster_barcode-*.whl

