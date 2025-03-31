target("RyuCommon")
	set_group("Ryu/Core")
	set_kind("headeronly")

	add_includedirs(".", { public = true })
	add_headerfiles("**.h")
target_end()
