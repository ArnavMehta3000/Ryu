set_xmakever("2.9.5")
add_rules("mode.debug", "mode.release")
set_allowedmodes("debug", "release")

-- Project name and version
set_project("Ryu")
set_version("0.0.2")

-- Set C/C++ language version
set_languages("c17", "cxx23")

-- Include other xmake scripts
includes("Xmake/Packages.lua")
includes("Xmake/Options.lua")
includes("Xmake/Rules.lua")

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

-- Allow auto build before running
set_policy("run.autobuild", true)

-- Keep intermediate directories
set_policy("build.intermediate_directory", true)

-- Always update config files (fixes issues with CoreLogDefines.h)
set_policy("build.always_update_configfiles", true)

if is_mode("debug") then
	-- Enable preprocessor markers in debug mode
	set_policy("preprocessor.linemarkers", true)

	-- Enable hot reloading
	set_symbols("debug", "edit")
end

add_defines("UNICODE", "_UNICODE", "NOMINMAX", "NOMCX", "NOSERVICE", "NOHELP", "WIN32_LEAN_AND_MEAN")
add_links("user32.lib")

-- Add compilation success to all targets
add_tests("CompileSuccess", { build_should_pass = true, group = "Compilation" })

-- Include xmake projects
includes("**/xmake.lua")