target("RyuUtils")
	set_group("Ryu/Core")
	set_kind("static")
	add_includedirs(".", { public = true })
	add_files("**.cpp")
	add_headerfiles("**.h")

	add_packages("libassert", "uuid_v4")

	add_deps("RyuCommon")
target_end()
