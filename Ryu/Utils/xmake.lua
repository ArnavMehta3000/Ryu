target("RyuUtils")
	set_group("Ryu/Core")
	set_kind("static")
	add_includedirs(".", { public = true })
	add_files("**.cpp", { unity_group = "Utilities" })
	add_headerfiles("**.h")
	add_extrafiles("**.inl")

	add_packages("uuid_v4", "Elos")

	add_deps("RyuCommon")
	add_rules("c++.unity_build")
target_end()
