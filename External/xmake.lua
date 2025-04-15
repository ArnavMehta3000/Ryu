target("SimpleMath")
	set_group("Ryu/External")
	set_kind("static")
	add_headerfiles("External/SimpleMath/SimpleMath.h")
	add_files("External/SimpleMath/SimpleMath.cpp")
target_end()

-- A phony target to include all external source files
target("RyuExternals")
	set_group("Ryu/External")
	set_kind("phony")
	add_includedirs(".", { public = true })

	add_deps("SimpleMath", { public = true })
target_end()
