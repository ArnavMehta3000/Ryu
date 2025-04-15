target("RyuProfiling")
	set_group("Ryu/Core")
	set_kind("headeronly")
	add_includedirs(".", { public = true })
	add_headerfiles("**.h")
	add_deps("RyuCommon")
	add_options("ryu-enable-tracy-profiling", { public = true })
	
	if get_config("ryu-enable-tracy-profiling") then
		add_packages("tracy", { public = true })
	end
target_end()
