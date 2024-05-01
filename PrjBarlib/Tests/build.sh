premake5 gmake
make || { echo 'BUILD failed' ; exit 1; }
build/BarTests