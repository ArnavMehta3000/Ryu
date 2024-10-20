target("RyuLogger")
	set_group("Ryu/Core")
	set_kind("static")
	add_includedirs(".", { public = true })
	add_files("**.cpp")
	add_headerfiles("**.h")

	add_packages("fmt", { public = true })
	add_packages("libassert", { public = true })

	add_deps("RyuUtils")
target_end()
