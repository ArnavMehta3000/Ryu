target("RyuConfig")
	set_group("Ryu/Core")
	set_kind("static")
	add_includedirs(".", { public = true })
	add_headerfiles("**.h")
	add_files("**.cpp")
	add_packages("toml++", { public = true })
	add_deps("RyuCommon", "RyuUtils", "RyuLogger")
	add_rules("c++.unity_build")
target_end()
