target("RyuApp")
	set_group("Ryu/Core")
	set_kind("static")
	add_includedirs(".", { public = true })
	add_files("**.cpp", { unity_group = "Application" })
	add_headerfiles("**.h")

	add_packages("Elos", "entt", { public = true })
	add_deps(
		"RyuUtils",
		"RyuConfig",
		"RyuProfiling",
		"RyuMemory",
		"RyuWindow"
	)

	add_links("Dwmapi")
	add_rules("c++.unity_build")
target_end()
