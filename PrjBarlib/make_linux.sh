premake5 gmake2 --python-include-path=/usr/include/python3.13 \
	--python-lib-path=/usr/lib/x86_64-linux-gnu/ \
	--python-version=3.13
cd build
make config=python clean
make config=python Barlib
