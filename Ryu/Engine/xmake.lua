target("RyuEngine")
	set_group("Ryu")
	set_kind("static")
	add_includedirs(".", { public = true })
	add_files("**.cpp")
	add_headerfiles("**.h")

	-- Make monolithic engine library
	set_policy("build.merge_archive", true)

	add_deps(
		"RyuCore",
		"RyuGraphics",
		"RyuExternals")
target_end()
