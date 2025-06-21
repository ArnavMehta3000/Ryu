set_xmakever("3.0.1")
add_rules("mode.debug", "mode.release", "mode.releasedbg")

-- Project name and version
set_project("Ryu")
set_version("0.0.3", {build = "%Y%m%d%H%M"})

-- Set C/C++ language version
set_languages("c17", "cxx23")

-- Include other xmake scripts
add_moduledirs("Xmake/Modules")
add_plugindirs("Xmake/Plugins")
includes("Xmake/Packages.lua")
includes("Xmake/Options.lua")
includes("Xmake/Rules/ExportAPI.lua")
includes("Xmake/Rules/RyuGame.lua")
includes("Xmake/Rules/EnumToHeader.lua")

-- Update compile commands
-- add_rules("plugin.compile_commands.autoupdate", { outputdir = "build", lsp = "clangd" })

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

if is_mode("debug", "releasedbg") then
	add_defines("RYU_BUILD_DEBUG", "_DEBUG")
	set_runtimes("MDd")
else
	set_runtimes("MD")
end

-- Add common window definitions
add_defines("UNICODE", "_UNICODE", "NOMINMAX", "NOMCX", "NOSERVICE", "NOHELP", "WIN32_LEAN_AND_MEAN")
add_links("user32.lib")

-- Bring standard types namespace to global namespace
if has_config("use-std-types-globally") then
	add_defines("RYU_USING_STD_TYPES_GLOBALLY")
end

-- Add compilation success to all targets
add_tests("CompileSuccess", { build_should_pass = true, group = "Compilation" })

-- Add rule to export shared library
add_rules("ExportAPI")

-- Include xmake projects
includes("**/xmake.lua")
