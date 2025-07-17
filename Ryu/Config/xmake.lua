target("RyuConfig")
	set_group("Ryu/Core")
	set_kind("static")
	add_includedirs(".", { public = true })
	add_headerfiles("**.h")
	add_files("**.cpp", { unity_group = "Config" })
	add_packages("toml++", "Elos", { public = true })

	add_deps(
		"RyuCommon",
		"RyuUtils",
		"RyuLogger",
		"RyuProfiling"
	)
	if has_config("ryu-unity-build") then
		add_rules("c++.unity_build")
	end
target_end()
