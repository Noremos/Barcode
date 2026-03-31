.\premake5.exe --python-include-path=D:\WSpace\Python12\include --python-lib-path=D:\WSpace\Python12\libs --python-version=3.12 vs2022
cd build
msbuild /p:Configuration=python /p:Platform=x64 Barlib.vcxproj
call "../modules/python/make_package.bat" Python python.exe
copy Python\BarcodeProject\dist dist

@REM bash ../modules/python/make_package.sh Python python313
