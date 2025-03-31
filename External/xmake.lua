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

-- A phony target to include all external source files
target("RyuExternals")
	set_group("Ryu/External")
	set_kind("phony")
	add_includedirs(".", { public = true })

	add_deps(
		"SimpleMath",
		"StepTimer",
		{ public = true })
target_end()
