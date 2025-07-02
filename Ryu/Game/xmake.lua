target("RyuGame")
	set_group("Ryu/Core")
	set_kind("headeronly")
	add_includedirs(".", { public = true })
	add_headerfiles("**.h")
	add_packages("entt")
	add_deps("RyuCommon")

	if has_config("ryu-unity-build") then
		add_rules("c++.unity_build")
	end
target_end()
