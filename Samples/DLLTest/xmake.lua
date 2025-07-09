target("RyuDLLTestbench")
	--add_rules("RyuGame")  -- Build this target as the game project
	set_enabled(false)
	after_config(function(target)
		target:set("kind", "shared")
	end)
	add_files("**.cpp", { unity_group = "DLLTest" })
	--add_headerfiles("**.h")

	set_group("Ryu/Testing")
	add_includedirs(".")
	add_headerfiles("**.h")
	add_deps("RyuEngine", "RyuPlugin")
target_end()
