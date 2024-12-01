#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
OUTPUT_DIR="$1"

PYTHON_BIN="$2"
if [ -z "$PYTHON_BIN" ]; then
	PYTHON_BIN="python3"
fi

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
pip install dist/raster_barcode-*.whl

