target("RyuEditor")
	set_enabled(get_config("ryu-game-as-dll"))  -- Only enable the editor when the game is being built as a dll
	
	if not is_mode("debug", "releasedbg") then
		add_rules("win.sdk.application")
	end
	
	set_default(true)
	set_kind("binary")
	set_group("Ryu")

	add_includedirs(".")
	add_files("**.cpp", { unity_group = "Editor" })
	add_headerfiles("**.h")

	add_packages("imgui")
	add_deps("RyuEngine")
	add_rules("c++.unity_build")
target_end()
