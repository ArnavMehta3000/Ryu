target("SimpleMath")
	set_group("Ryu/External")
	set_kind("static")
	add_headerfiles("External/SimpleMath/SimpleMath.h")
	add_files("External/SimpleMath/SimpleMath.cpp")
target_end()

target("GameInput")
	set_group("Ryu/External")
	set_kind("headeronly")
	add_extrafiles("External/GameInput/README.md")
	add_headerfiles("External/GameInput/**.h")
	add_links("$(scriptdir)/External/External/GameInput/GameInput.lib", { public = true })
target_end()

-- A phony target to include all external source files
target("RyuExternals")
	set_group("Ryu/External")
	set_kind("phony")
	add_includedirs(".", { public = true })

	add_deps("SimpleMath", { public = true })
target_end()

target("AngelScript")
	set_group("Ryu/External")
	set_kind("static")
	add_defines("ANGELSCRIPT_EXPORT", "_CRT_SECURE_NO_WARNINGS")
	
	add_includedirs("./External/AngelScript/angelscript", { public = true})
	add_includedirs("./External/AngelScript/add_on", { public = true})
	add_includedirs("./External/AngelScript/angelscript/include", { public = true })

	add_headerfiles("./External/AngelScript/angelscript/include/angelscript.h", { public = true })
	add_headerfiles("./External/AngelScript/add_on/**.h", { public = true})
	
	add_files("External/AngelScript/angelscript/source/**.cpp")
	add_files("External/AngelScript/add_on/**.cpp")
	add_files("External/AngelScript/angelscript/source/as_callfunc_x64_msvc_asm.asm")

	remove_files("External/AngelScript/angelscript/source/*_arm.cpp")
	remove_files("External/AngelScript/angelscript/source/*_gcc.cpp")
	remove_files("External/AngelScript/angelscript/source/*_mingw.cpp")
target_end()
