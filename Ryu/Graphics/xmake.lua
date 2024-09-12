target("RyuGraphics")
	set_kind("static")
	set_group("Ryu")

	add_rules(
		"IncludeConfigs",
		"CommonPackages")

	add_includedirs("..", { public = true })
	add_files("**.cpp")
	add_headerfiles("**.h")

	add_deps("RyuCore")
	add_links("RyuCore")
target_end()