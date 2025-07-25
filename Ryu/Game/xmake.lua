target("RyuGame")
	set_group("Ryu/Core")
	set_kind("static")
	add_includedirs(".", { public = true })
	add_headerfiles("**.h", "**.inl")
	add_files("**.cpp", { unity_group = "Game" })
	add_packages("entt", { public = true })
	add_deps("RyuUtils", "RyuLogger")

	if has_config("ryu-unity-build") then
		add_rules("c++.unity_build")
	end
target_end()
