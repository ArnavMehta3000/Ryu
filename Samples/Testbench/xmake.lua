target("RyuTestbench")
	add_rules("RyuGame")  -- Build this target as the game project

	set_default(not get_config("ryu-build-with-editor"))
	set_group("Ryu/Testing")
	add_includedirs(".")
	add_files("Testbench/**.cpp", { unity_group = "Testbench" })
	add_extrafiles("Config/**.toml")
	add_headerfiles("Testbench/**.h")

	add_deps(
		"RyuEngine",
		"GameInput"  -- External deps
	)

	if has_config("ryu-unity-build") then
		add_rules("c++.unity_build")
	end

	-- For DLL builds, export symbols
    if has_config("ryu-enable-hot-reload") and has_config("ryu-build-with-editor") then
        add_defines("RYU_GAME_EXPORTS")
    end
target_end()
