target("RyuConfig")
	set_group("Ryu/Core")
	set_kind("headeronly")
	add_includedirs(".", { public = true })
	add_headerfiles("**.h")
	add_files("**.cpp")

	add_deps("RyuCommon", "RyuUtils")
target_end()
