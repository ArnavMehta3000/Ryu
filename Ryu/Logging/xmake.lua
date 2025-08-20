target("RyuLogging")
	set_group("Ryu/Core")
	set_kind("static")
	add_includedirs(".", { public = true })
	add_files("**.cpp", { unity_group = "Logging" })
	add_headerfiles("**.h", "**.inl")

	add_packages("spdlog", { public = true })
	add_deps("RyuUtils", "RyuGlobals")
	add_options("ryu-log-level")

	if has_config("ryu-unity-build") then
		add_rules("c++.unity_build")
	end
target_end()
