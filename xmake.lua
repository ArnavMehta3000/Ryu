set_xmakever("2.9.4")
add_rules("mode.debug", "mode.release")
set_allowedmodes("debug", "release")

-- Project name and version
set_project("Ryu")
set_version("0.0.2")

-- Set C/C++ language version
set_languages("c17", "cxx23")

-- Include other xmake scripts
includes("Scripts/Packages.lua")
includes("Scripts/Options.lua")
includes("Scripts/Rules.lua")

-- Build debug mode by default
set_defaultmode("debug")

-- Platform and architecture - only support x64 Windows
set_allowedplats("windows")
set_allowedarchs("windows|x64")

-- Enable all warnings and handle them as compilation errors
set_policy("build.warning", true)
set_warnings("all", "extra")

-- Allow auto build before running
set_policy("run.autobuild", true)

-- Keep intermediate directories
set_policy("build.intermediate_directory", true)

add_defines("UNICODE", "_UNICODE", "NOMINMAX", "NOMCX", "NOSERVICE", "NOHELP", "WIN32_LEAN_AND_MEAN")
add_links("user32.lib")

-- Add compilation success to all targets
add_tests("CompileSuccess", {build_should_pass = true, group = "Compilation"})

-- Include xmake projects
includes("**/xmake.lua")