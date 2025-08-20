target("RyuEvent")
	set_group("Ryu/Core")
	set_kind("static")
	add_includedirs(".", { public = true })
	add_files("**.cpp", { unity_group = "Event" })
	add_headerfiles("**.h")
	add_extrafiles("**.inl")

	add_packages("Elos")
	add_deps("RyuCommon")

	if has_config("ryu-unity-build") then
		add_rules("c++.unity_build")
	end
target_end()
