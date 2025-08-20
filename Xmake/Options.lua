option("ryu-log-level")
	set_showmenu(true)
	set_default("default")
	set_values("default", "trace", "debug", "info", "warn")
	set_category("root Ryu/Logging")
	set_description("Compile time switch to set log level")

	after_check(function (option)
		local value = string.lower(option:value())
		local define = ""  -- define to add

		if value == "trace" then
			define = "RYU_LOG_LEVEL_TRACE"
		elseif value == "debug" then
			define = "RYU_LOG_LEVEL_DEBUG"
		elseif value == "info" then
			define = "RYU_LOG_LEVEL_INFO"
		elseif value == "warn" then
			define = "RYU_LOG_LEVEL_WARN"
		else
			define = "default"
		end

		if define ~= "default" then
			option:add("defines", define, { public = true })
		end
		cprint(format("Compile-time log level set to ${cyan}%s${clear}", value))
	end)
option_end()

option("ryu-throw-on-fail-hresult")
	set_showmenu(true)
	set_default(true)
	set_description("Assert on failure of HRESULT")
	set_category("root Ryu/Graphics")
option_end()

option("ryu-enable-tracy-profiling")
	set_showmenu(true)
	set_default(false)
	set_description("Enable profiling")
	set_category("root Ryu/Profiling")
	add_defines("RYU_PROFILING_ENABLED", "TRACY_ENABLE")
option_end()

option("ryu-build-with-editor")
    set_default(true)
    set_showmenu(true)
    set_category("root Ryu/Game")
    set_description("Enable the game editor")
option_end()

option("ryu-unity-build")
    set_default(true)
    set_showmenu(true)
    set_category("root Ryu/Build")
    set_description("Compile using unity build")
option_end()
