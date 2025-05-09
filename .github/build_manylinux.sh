PY_DOT_VERSION=$1

PYVER=$(echo ${PY_DOT_VERSION} | tr -d .)
PYROOT=/opt/python/cp${PYVER}-cp${PYVER}
PYBIN=${PYROOT}/bin/python3
PYINCLUDE=${PYROOT}/include/python${PY_DOT_VERSION}


echo --BUILDING for Python ${PYVER} ${PY_DOT_VERSION}--
echo --PYTHON_ROOT=${PYROOT}--
echo --PYTHON_BIN=${PYBIN}--
echo --PYTHON_INCLUDE=${PYINCLUDE}--

yum install -y patchelf
curl -L -o premake5.tar.gz https://github.com/premake/premake-core/releases/download/v5.0.0-alpha16/premake-5.0.0-alpha16-linux.tar.gz
tar -xvf premake5.tar.gz
mv premake5 /usr/local/bin/



cd PrjBarlib
premake5 gmake2 --python-include-path=${PYINCLUDE} --python-version=${PY_DOT_VERSION}

cd build
make config=python Barlib
echo --DONE--

bash ../modules/python/make_package.sh Python ${PYBIN}

mkdir -p /project/dist
${PYBIN} -m pip install --upgrade auditwheel
auditwheel repair --plat manylinux_2_28_x86_64 ./Python/BarcodeProject/dist/*.whl -w /project/dist