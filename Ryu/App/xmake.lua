target("RyuApp")
	set_group("Ryu/Core")
	set_kind("static")
	add_includedirs(".", { public = true })
	add_files("**.cpp")
	add_headerfiles("**.h")

	add_deps("RyuUtils", "RyuInput")

	add_links("Dwmapi")
target_end()
