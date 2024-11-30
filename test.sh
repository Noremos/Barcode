source ./.venv/bin/activate

pip uninstall -y raster_barcode
pip install PrjBarlib/build/Python/BarcodeProject/dist/raster_barcode-1.0.0-py3-none-any.whl
python PrjBarlib/modules/python/example.py