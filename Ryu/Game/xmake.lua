target("RyuGame")
	set_group("Ryu/Core")
	set_kind("static")
	add_includedirs(".", { public = true })
	add_headerfiles("**.h")
	add_files("**.cpp")
	add_extrafiles("**.inl")
	add_packages("entt", "libassert" , { public = true })

	add_deps(
		"RyuUtils",
		"RyuLogger",
		"RyuScripting",
		"RyuEvents")
target_end()
