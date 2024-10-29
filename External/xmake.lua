-- A phony target to include all external source files
target("RyuExternals")
	set_group("Ryu/External")
	set_kind("phony")
	add_includedirs(".", { public = true })
	add_headerfiles("**.h")
	add_files("**.cpp")
target_end()
