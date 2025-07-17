target("RyuEngine")
	set_group("Ryu")
	set_kind("static")
	add_includedirs(".", { public = true })
	add_files("**.cpp", { unity_group = "Engine" })
	add_headerfiles("**.h")

	-- Make monolithic engine library
	set_policy("build.merge_archive", true)

	add_packages("entt")

	add_deps(
		"RyuGlobals",
		"RyuUtils",
		"RyuConfig",
		"RyuLogger",
		"RyuApp",
		"RyuGame",
		"RyuPlugin",
		"RyuGraphics",
		"RyuMath",
		"RyuProfiling",
		"RyuScripting",
		"RyuExternals", { public = true })

	if has_config("ryu-unity-build") then
		add_rules("c++.unity_build")
	end

	add_links("runtimeobject")
target_end()
