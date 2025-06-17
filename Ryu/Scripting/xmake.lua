target("RyuScripting")
	set_group("Ryu/Core")
	set_kind("static")
	add_includedirs(".", { public = true })
	add_headerfiles("**.h")
	add_files("**.cpp", { unity_group = "Scripting" })
	
	add_deps(
		"RyuLogger", 
		"RyuProfiling"		
	)

	add_deps("AngelScript", { public = true })
	
	add_rules("c++.unity_build")
target_end()
