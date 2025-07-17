target("RyuGlobals")
	set_group("Ryu/Core")
	set_kind("static")
	add_includedirs(".", { public = true })
	add_files("**.cpp", { unity_group = "Globals" })
	add_headerfiles("**.h")

	add_deps(
		"RyuCommon",
		"RyuUtils"
	)
target_end()
