target("RyuEvents")
	set_group("Ryu/Core")
	set_kind("headeronly")
	add_includedirs(".", { public = true })
	add_headerfiles("**.h")

	add_packages("uuid_v4")

	add_deps("RyuUtils")
target_end()
