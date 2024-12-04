premake5 gmake2 --python-include-path=/usr/include/python3.13 \
	--python-lib-path=/usr/lib/x86_64-linux-gnu/ \
	--python-version=3.13
cd build
make config=python clean
make config=python Barlib

	cp -rf %[%{prj.location}/../modules/python/BarcodeProject] %[%{!cfg.targetdir}/BarcodeProject]",
	"{COPYFILE} %[%{!cfg.buildtarget.abspath}]  %[%{!cfg.targetdir}/BarcodeProject/raster_barcode/]",
}

if os.host() == "windows" then
	postbuildcommands {
		"%[%{prj.location}/../modules/python/make_package.bat] %[%{!cfg.targetdir}/BarcodeProject/raster_barcode/] " .. "python.exe",
	}
else
	postbuildcommands {
		"chmod +x %[%{prj.location}/../modules/python/make_package.sh]",
		"%[%{prj.location}/../modules/python/make_package.sh] %[%{!cfg.targetdir}/BarcodeProject/raster_barcode/] " .. pythonBin,
	}