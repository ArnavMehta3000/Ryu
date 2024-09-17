target("RyuTest")
	add_rules("IncludeConfigs", "CommonPackages", "RyuPlugin")
	
	add_includedirs("..", { public = true })
	add_files("**.cpp")
	add_headerfiles("**.h")
target_end()