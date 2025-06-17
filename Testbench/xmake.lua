target("RyuTestbench")
	add_rules("RyuGame")  -- Build this target as the game project

	set_default(not get_config("ryu-game-as-dll"))
	set_group("Ryu/Testing")

	add_includedirs(".")
	add_files("Testbench/**.cpp")
	add_extrafiles("Scripts/**.as")
	add_headerfiles("Testbench/**.h")
	add_deps("RyuEngine", "RyuWindow")
target_end()
