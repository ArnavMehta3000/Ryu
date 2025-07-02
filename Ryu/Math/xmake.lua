target("RyuMath")
	set_group("Ryu/Core")
	set_kind("static")
	add_includedirs(".", { public = true })
	add_headerfiles("**.h")
	add_files("**.cpp", { unity_group = "Math" })

	add_deps(
		"RyuCommon",
		"RyuExternals"
	)
	if has_config("ryu-unity-build") then
		add_rules("c++.unity_build")
	end
target_end()
