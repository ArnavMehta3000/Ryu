target("RyuTestbench")
	-- If building as DLL
	if has_config("ryu-game-as-dll") then
		set_kind("shared")
		add_defines("RYU_GAME_AS_DLL")
	else
		set_kind("binary")
		
		-- Use WinMain only in release mode
		if not is_mode("debug", "releasedbg") then
			add_rules("win.sdk.application")
		end
	end

	set_default(not get_config("ryu-game-as-dll"))
	set_group("Ryu/Testing")

	add_includedirs(".")
	add_files("Testbench/**.cpp")
	add_extrafiles("Scripts/**.as")
	add_headerfiles("Testbench/**.h")
	add_deps("RyuEngine")
target_end()


target("RyuTestbenchRunner")
	if not is_mode("debug", "releasedbg") then
		add_rules("win.sdk.application")
	end

	set_default(get_config("ryu-game-as-dll"))
	set_kind("binary")
	set_group("Ryu/Testing")

	add_includedirs(".")
	add_files("Testbench/Main.cpp")
	add_deps("RyuTestbench")
target_end()
