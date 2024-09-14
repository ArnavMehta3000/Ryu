target("RyuInput")
	add_rules("IncludeConfigs", "CommonPackages", "RyuPlugin")

	set_default(true)
	set_kind("shared")
	set_group("Ryu/Plugins/Engine")

	add_includedirs("..", { public = true })
	add_files("**.cpp")
	add_headerfiles("**.h")

	add_deps("RyuCore")
	add_links("RyuCore")
target_end()