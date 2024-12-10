rm -rf ./.venv
python3 -m venv ../.venv
source ../.venv/bin/activate

pip install ../dist/ImageTopoDec-*.whl
pip install opencv-python
python3 modules/python/example.py