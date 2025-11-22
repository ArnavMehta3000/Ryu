target("RyuGame")
	set_group("Ryu/Core")
	set_kind("static")
	add_includedirs(".", { public = true })
	add_headerfiles("**.h", "**.inl")

	add_files("Game/World/**.cpp", { unity_group = "GameWorld" })
	add_files("Game/Components/**.cpp", { unity_group = "GameComponents" })

	add_packages("entt", "toml++", { public = true })

	add_deps(
		"RyuUtils",
		"RyuMath",
	 	"RyuLogging",
		"RyuEvent"
	)

	if has_config("ryu-unity-build") then
		add_rules("c++.unity_build")
	end
target_end()
