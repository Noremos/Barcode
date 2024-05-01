premake5 gmake
make config=debug BarTests || { echo 'BUILD failed' ; exit 1; }
build/BarTests