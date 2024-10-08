target("RyuInput")
	set_group("Ryu/Core")
	set_kind("static")
	add_includedirs(".", { public = true })
	add_files("**.cpp")
	add_headerfiles("**.h")

	add_deps(
		"RyuCommon",
		"RyuEvents"
	)
target_end()
