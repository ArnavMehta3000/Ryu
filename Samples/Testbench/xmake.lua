target("RyuTestbench")
	add_rules("RyuGame")  -- Build this target as the game project

	set_default(not get_config("ryu-build-with-editor"))
	set_group("Ryu/Testing")
	add_packages()
	add_includedirs(".")
	add_files("Testbench/**.cpp", { unity_group = "Testbench" })
	add_extrafiles("Scripts/**.as")
	add_headerfiles("Testbench/**.h")

	add_deps(
		"RyuEngine",
		"GameInput"  -- External deps
		--,"RyuTestbenchEnginePlugin"
	)
target_end()
