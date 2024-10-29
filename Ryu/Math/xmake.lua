target("RyuMath")
	set_group("Ryu/Core")
	set_kind("headeronly")
	add_includedirs(".", { public = true })
	add_headerfiles("**.h")

	add_deps("RyuExternals")
target_end()
