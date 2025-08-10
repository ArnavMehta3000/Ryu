target("RyuConfig")
	set_group("Ryu/Core")
	set_kind("static")
	add_includedirs(".", { public = true })
	add_headerfiles("**.h", "**.inl")
	add_files("**.cpp", { unity_group = "Config" })

	add_packages("toml++", "Elos", "cli11", { public = true })

	add_deps(
		"RyuCommon",
		"RyuUtils",
		"RyuLogging",
		"RyuProfiling"
	)
	if has_config("ryu-unity-build") then
		add_rules("c++.unity_build")
	end
target_end()
