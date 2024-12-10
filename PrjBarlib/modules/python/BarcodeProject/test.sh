source ../.venv/bin/activate
python -m build
# stubgen -m libbarpy
pip uninstall -y ImageTopoDec
pip install dist/raster_barcode-1.0.0-py3-none-any.whl
ls ../.venv/lib/python3.13/site-packages/raster_barcode