target("SimpleMath")
	set_group("Ryu/External")
	set_kind("static")
	add_headerfiles("External/SimpleMath/SimpleMath.h")
	add_files("External/SimpleMath/SimpleMath.cpp")
target_end()

target("StepTimer")
	set_group("Ryu/External")
	set_kind("headeronly")
	add_headerfiles("External/StepTimer/StepTimer.h")
target_end()

target("AngelScriptAddOns")
	set_group("Ryu/External")
	set_kind("static")
	add_packages("angelscript", { public = true })
	add_headerfiles("External/AngelScript/**.h")
	add_files("External/AngelScript/**.cpp")
	add_defines("_CRT_SECURE_NO_WARNINGS")  -- Remove library deprecated warnings
target_end()


-- A phony target to include all external source files
target("RyuExternals")
	set_group("Ryu/External")
	set_kind("phony")
	add_includedirs(".", { public = true })

	add_deps(
		"SimpleMath",
		"StepTimer",
		"AngelScriptAddOns",
		{ public = true })

	add_links("AngelScriptAddOns")
target_end()
