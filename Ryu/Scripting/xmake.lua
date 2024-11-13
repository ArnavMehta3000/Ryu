target("RyuScripting")
	set_group("Ryu/Core")
	set_kind("static")
	add_includedirs(".", { public = true })
	add_headerfiles("**.h")
	add_files("**.cpp")
	add_extrafiles("**.inl")
	add_packages("angelscript", "libassert" , { public = true })

	add_deps(
		"RyuUtils",
		"RyuLogger",
		"RyuEvents",
		"RyuExternals")
target_end()
