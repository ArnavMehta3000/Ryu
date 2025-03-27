target("RyuApp")
	set_group("Ryu/Core")
	set_kind("static")
	add_includedirs(".", { public = true })
	add_files("**.cpp")
	add_headerfiles("**.h")

	add_packages("Elos", { public = true })
	add_deps("RyuUtils")

	add_links("Dwmapi")
	add_rules("c++.unity_build")
target_end()
