target("RyuEngine")
	set_group("Ryu")
	set_kind("static")
	add_includedirs(".", { public = true })
	add_files("**.cpp", { unity_group = "Engine" })
	add_headerfiles("**.h")

	-- Make monolithic engine library
	set_policy("build.merge_archive", true)

	add_packages("entt")

	add_deps(
		"RyuUtils",
		"RyuConfig",
		"RyuLogger",
		"RyuApp",
		"RyuGame",
		"RyuGraphics",
		"RyuMath",
		"RyuProfiling",
		"RyuScripting",
		"RyuExternals", { public = true })
		add_rules("c++.unity_build")
target_end()
