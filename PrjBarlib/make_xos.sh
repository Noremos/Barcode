premake5 gmake --python-include-path=/opt/homebrew/opt/python@3.13/Frameworks/Python.framework/Versions/3.13/include/python3.13 \
	--python-lib-path=/opt/homebrew/opt/python@3.13/Frameworks/Python.framework/Versions/3.13/lib/ \
	--python-version=3.13
if [[ $? -ne 0 ]] ; then
    exit 1
fi

cd build
make config=python clean
make config=python Barlib
if [[ $? -ne 0 ]] ; then
    exit 1
fi

rm -rf Python/BarcodeProject
bash ../modules/python/make_package.sh Python python3
if [[ $? -ne 0 ]] ; then
    exit 1
fi

cp -r ./Python/BarcodeProject/dist ../../dist