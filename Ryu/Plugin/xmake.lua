target("RyuPlugin")
	set_group("Ryu/Core")
	set_kind("static")
	add_includedirs(".", { public = true })
	add_files("**.cpp", { unity_group = "Plugin" })
	add_headerfiles("**.h")
	add_packages("entt")
	add_deps("RyuUtils")

	if has_config("ryu-unity-build") then
		add_rules("c++.unity_build")
	end
target_end()
