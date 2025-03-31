set_xmakever("2.9.7")
add_rules("mode.debug", "mode.release")
set_allowedmodes("debug", "release")

-- Project name and version
set_project("Ryu")
set_version("0.0.3")

-- Set C/C++ language version
set_languages("c17", "cxx23")

-- Include other xmake scripts
add_moduledirs("Xmake/Modules")
includes("Xmake/Tasks.lua")
includes("Xmake/Packages.lua")
includes("Xmake/Options.lua")
includes("Xmake/Rules/RyuPlugin.lua")
includes("Xmake/Rules/ExportAPI.lua")
includes("Xmake/Rules/CopyToBuildDir.lua")
includes("Xmake/Rules/AddScriptPathDefine.lua")
includes("Xmake/Rules/EnumAutogen.lua")

-- Build debug mode by default
set_defaultmode("debug")

-- Platform and architecture - only support x64 Windows
set_allowedplats("windows")
set_allowedarchs("windows|x64")

-- Only allow MSVC
set_toolchains("msvc")

-- Enable all warnings and handle them as compilation errors
set_policy("build.warning", true)
set_warnings("all", "extra")

-- Set policies
set_policy("run.autobuild", true)
set_policy("build.intermediate_directory", true)
set_policy("package.requires_lock", false)
set_policy("build.always_update_configfiles", false)

if is_mode("debug") then
	-- Enable preprocessor markers in debug mode
	set_policy("preprocessor.linemarkers", true)
	add_defines("RYU_BUILD_DEBUG")
	set_symbols("debug", "embed")
end

if is_mode("release") then
	set_symbols("hidden")
	set_strip("all")
end

add_defines("UNICODE", "_UNICODE", "NOMINMAX", "NOMCX", "NOSERVICE", "NOHELP", "WIN32_LEAN_AND_MEAN")
add_links("user32.lib")
--set_runtimes(is_mode("debug") and "MDd" or "MD")

-- Bring standard types namespace to global namespace
if has_config("use-std-types-globally") then
	add_defines("RYU_USING_STD_TYPES_GLOBALLY")
end

-- Add compilation success to all targets
add_tests("CompileSuccess", { build_should_pass = true, group = "Compilation" })

-- Add rule to export shared library
add_rules("ExportAPI")

local configDir = path.join(os.projectdir(), "Config")
configDir = configDir:gsub("\\", "/")
local defineValue = "RYU_ROOT_CONFIG_DIR=\"" .. configDir .. "\""
add_defines(defineValue)

-- Include xmake projects
includes("**/xmake.lua")
