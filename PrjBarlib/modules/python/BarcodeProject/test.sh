source ../.venv/bin/activate
python -m build
# stubgen -m libbarpy
pip uninstall -y ImageTopoDec
pip install dist/ImageTopoDec-1.0.2-py3-none-any.whl
ls ../.venv/lib/python3.13/site-packages/ImageTopoDec