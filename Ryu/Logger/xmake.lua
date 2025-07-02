target("RyuLogger")
	set_group("Ryu/Core")
	set_kind("static")
	add_includedirs(".", { public = true })
	add_files("**.cpp", { unity_group = "Logger" })
	add_headerfiles("**.h")

	add_packages("fmt", { public = true })
	add_deps("RyuUtils", "RyuGlobals")
	add_options(
		"ryu-log-trace-enabled",
		"ryu-log-debug-enabled",
		"ryu-log-info-enabled",
		"ryu-log-warn-enabled")

	-- Compile time switch to toggle logging
	if get_config("ryu-log-trace-enabled") then
		add_defines("RYU_LOG_ENABLED_TRACE", { public = true })
	end
	if get_config("ryu-log-debug-enabled") then
		add_defines("RYU_LOG_ENABLED_DEBUG", { public = true })
	end
	if get_config("ryu-log-info-enabled") then
		add_defines("RYU_LOG_ENABLED_INFO", { public = true })
	end
	if get_config("ryu-log-warn-enabled") then
		add_defines("RYU_LOG_ENABLED_WARN", { public = true })
	end

	add_rules("EnumToHeader",
		{
			root = path.join(os.projectdir(), "Ryu", "Enums"),
			files =
			{
				"LogLevel.json"
			},
			force = false
		})

	if has_config("ryu-unity-build") then
		add_rules("c++.unity_build")
	end
target_end()
