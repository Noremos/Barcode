
-- Options
newoption {
	trigger = "disable-postbuild",
	description = "Do not add postbuild actions"
}

newoption {
	trigger = "python-include-path",
	value = "<PATH>",
	description = "A path to the Python include folder. For example: /opt/homebrew/Cellar/python@3.13/3.13.0_1/include",
}
newoption {
	trigger = "python-lib-path",
	value = "<PATH>",
	description = "A path to the Python lib file. For example: /opt/homebrew/Cellar/python@3.13/3.13.0_1/lib",
}

newoption {
	trigger = "python-version",
	value = "<VERSION>",
	description = "Python version. For example: 3.13",
}



function queryTerminal(command)
    local success, handle = pcall(io.popen, command)
    if not success then
        return ""
    end

    result = handle:read("*a")
    handle:close()
    result = string.gsub(result, "\n$", "") -- remove trailing whitespace
    return result
end

function getPythonPath()
    local p = queryTerminal('python -c "import sys; import os; print(os.path.dirname(sys.executable))"')

    -- sanitize path before returning it
    p = string.gsub(p, "\\\\", "\\") -- replace double backslash
    p = string.gsub(p, "\\", "/") -- flip slashes
    return p
end

function getPythonLib()
    return queryTerminal("python -c \"import sys; import os; import glob; path = os.path.dirname(sys.executable); libs = glob.glob(path + '/libs/python*'); print(os.path.splitext(os.path.basename(libs[-1]))[0]);\"")
end

-- pythonPath      = getPythonPath()
-- pythonIncludePath = pythonPath .. "/include/"
-- pythonLibPath     = pythonPath .. "/libs/"
-- pythonLib         = getPythonLib()

-- if pythonPath == "" or pythonLib == "" then
--     error("Failed to find python path!")
-- else
--     print("Python includes: " .. pythonIncludePath)
--     print("Python libs: " .. pythonLibPath)
--     print("lib: " .. pythonLib)
-- end

function getPythonNameInDirsro(python_version)
	if os.host() == "windows" then
		python_version = python_version:gsub("%.", "")
	end

	return "python" .. python_version
end

function setPythonSetup()
	defines { "_PYD" }

	includedirs { "include", "modules/pybind11/include" }
	includedirs {  _OPTIONS["python-include-path"] }

	python_version = getPythonNameInDirsro(_OPTIONS["python-version"])

	libdirs { _OPTIONS["python-lib-path"] }
	links { python_version } -- Link against the Python 3.10 library
end

workspace "Barcode"
	configurations { "Debug", "dll", "Python", "PythonDebug" }
	location "build"

	-- Define supported configurations
	filter { "configurations:Debug", "action:vs*" }
		defines { "DEBUG" }
		symbols "On"
		targetdir "build/Debug/"

	filter { "configurations:Release" }
		defines { "NDEBUG" }
		optimize "On"
		targetdir "build/Release/"

	filter { "configurations:dll" }
		targetdir "build/dll/"

	filter { "configurations:Python" }
		targetdir "build/Python/"
		setPythonSetup()

	filter { "configurations:PythonDebug" }
		targetdir "build/PythonDebug/"
		setPythonSetup()

	filter { "action:vs*" }
		defines { "COMMON_LANGUAGE_RUNTIME_DLLCVLESS=No", "COMMON_LANGUAGE_RUNTIME_DLL=No", "COMMON_LANGUAGE_RUNTIME_PYTHON=No", "COMMON_LANGUAGE_RUNTIME_PYTHONDEBUG=No" }

	if os.host() == "windows" then
		systemversion("latest")
		system      "windows"
		architecture "x64"

		symbolspath '$(TargetName).pdb'

	elseif os.host() == "linux" then
		system      "linux"
		architecture "x64"

	else -- MACOSX
		system      "macosx"
	end

project "Barlib"
	kind "SharedLib" -- Equivalent to add_library(... MODULE ...) in CMake
	language "C++"
	targetname "barpy"
	cppdialect "C++20"


	enable_postbuild = true
	print("Python version: " .. _OPTIONS["python-version"])
	if _OPTIONS["disable-postbuild"] then
		print("Disable postbuild")
		enable_postbuild = false
	end

	if os.host() == "windows" then
		targetextension ".pyd"
	else
		targetextension ".so"
	end

	files {
		-- No group source files (empty in the original CMake)
		"source/barclasses.cpp",
		"source/barcodeCreator.cpp",
		"source/component.cpp",
		"source/hole.cpp",
		"extra/Barpy.cpp"
	}

	-- This part handles additional dependencies
	-- Specify additional libraries required
	-- links { "additional_dependency" }  -- Uncomment and replace with actual dependencies if any

	-- Specific compiler options can be set here based on configuration.
	filter "configurations:Debug or PythonDebug"
		buildoptions { "-g" }

	filter "configurations:Release or Python"
		buildoptions { "-O3", "-Wall", "-g" }

	filter "system:windows"
		-- If specific Windows configurations are needed,
		-- this section could be expanded.

	-- Ensure that solution folders are defined (useful in IDEs)
	-- Solution folder names can be assigned if necessary but isn't in the original CMake file.
	-- `filter { }` settings allow cleaning up the output of the build configurations.

	if enable_postbuild then
		print("Enabling postbuild commands")
		pythonBin = getPythonNameInDirsro(_OPTIONS["python-version"])
		projectDir = "%[%{!cfg.targetdir}/BarcodeProject/]"
		libraryDir =  projectDir .. "/raster_barcode/]"

		filter "configurations:Python or PythonDebug"
			postbuildcommands
			{
				"{RMDIR} %[%{!cfg.targetdir}/BarcodeProject]",
				"{COPYDIR} %[%{prj.location}/modules/python/BarcodeProject] %[%{!cfg.targetdir}/BarcodeProject]",
				"{COPYFILE} %[%{!cfg.buildtarget.abspath}]  %[%{!cfg.targetdir}/BarcodeProject/raster_barcode/]",
			}

			if os.host() == "windows" then
				postbuildcommands {
					"%[%{prj.location}/modules/python/make_package.bat] %[%{!cfg.targetdir}/BarcodeProject/raster_barcode/] " .. pythonBin .. ".exe",
				}
			else
				postbuildcommands {
					"chmod +x %[%{prj.location}/modules/python/make_package.sh]",
					"%[%{prj.location}/modules/python/make_package.sh] %[%{!cfg.targetdir}/BarcodeProject/raster_barcode/] " .. pythonBin,
				}
			end
	end


	-- pythonBin = "python" .. _OPTIONS["python-version"]
	-- outProjectDir = "%{!cfg.targetdir}/BarcodeProject/"
	-- outLibraryDir =  outProjectDir .. "raster_barcode/"

	-- srcPythonModules = "%[%{prj.location}/modules/python/"

	-- filter "configurations:Python or PythonDebug"
	-- 	postbuildcommands
	-- 	{
	-- 		"{RMDIR} " .. outProjectDir,
	-- 		"{COPYDIR} %[" .. srcPythonModules .. "BarcodeProject] " .. outProjectDir,
	-- 		"{COPYFILE} %[%{!cfg.buildtarget.abspath}] %[" .. outLibraryDir .. "]",
	-- 	}

	-- 	if os.host() == "windows" then
	-- 		postbuildcommands {
	-- 			srcPythonModules .."make_package.bat " .. outLibraryDir .. " " .. pythonBin .. ".exe",
	-- 		}
	-- 	else
	-- 		postbuildcommands {
	-- 			scriptPath = srcPythonModules .. "make_package.sh]",
	-- 			"chmod +x " .. scriptPath",
	-- 			scriptPath .. " " .. outLibraryDir .. " " .. pythonBin,
	-- 		}
	-- 	end
	-- 	postbuildcommands {
	-- 		pythonBin .. srcPythonModules .."correct_types.py " .. outLibraryDir .. "libbarpy.pyi",
	-- 	}

