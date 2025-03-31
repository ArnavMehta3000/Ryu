target("RyuLogger")
	set_group("Ryu/Core")
	set_kind("static")
	add_includedirs(".", { public = true })
	add_files("**.cpp", { unity_group = "Logger" })
	add_headerfiles("**.h")

	add_packages("fmt", { public = true })
	add_packages("libassert", { public = true })

	add_deps("RyuUtils")
	add_rules("c++.unity_build")
target_end()
