target("RyuApp")
	set_group("Ryu/Core")
	set_kind("static")
	add_includedirs(".", { public = true })
	add_files("**.cpp", { unity_group = "Application" })
	add_headerfiles("**.h")

	add_packages("Elos", "entt")
	add_deps(
		"RyuGlobals",
		"RyuUtils",
		"RyuConfig",
		"RyuProfiling",
		"RyuWindow"
	)

	add_links("Dwmapi")
	if has_config("ryu-unity-build") then
		add_rules("c++.unity_build")
	end
target_end()
