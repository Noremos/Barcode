premake5 gmake2 --python-include-path=/opt/homebrew/opt/python@3.13/Frameworks/Python.framework/Versions/3.13/include/python3.13 \
	--python-lib-path=/opt/homebrew/opt/python@3.13/Frameworks/Python.framework/Versions/3.13/lib/ \
	--python-version=3.13
cd build
make config=python clean
make config=python Barlib
