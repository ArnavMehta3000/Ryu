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
		cprint(format("(config) Compile-time log level: ${cyan}%s${clear}", value))
	end)
option_end()

option("ryu-throw-on-fail-hresult")
	set_showmenu(true)
	set_default(true)
	set_description("Assert on failure of HRESULT")
	set_category("root Ryu/Graphics")

	after_check(function (option)
		cprint(format("(config) Throw on failure of HRESULT: ${cyan}%s${clear}", option:value()))
	end)
option_end()

option("ryu-enable-tracy-profiling")
	set_showmenu(true)
	set_default(false)
	set_description("Enable profiling")
	set_category("root Ryu/Profiling")
	add_defines("RYU_PROFILING_ENABLED", "TRACY_ENABLE")
	
	after_check(function (option)
		cprint(format("(config) Tracy profiling enabled: ${cyan}%s${clear}", option:value()))
	end)
option_end()

option("ryu-build-with-editor")
    set_default(true)
    set_showmenu(true)
    set_category("root Ryu/Game")
    set_description("Enable the game editor")

	after_check(function (option)
		cprint(format("(config) Game editor enabled: ${cyan}%s${clear}", option:value()))
	end)
option_end()

option("ryu-unity-build")
    set_default(true)
    set_showmenu(true)
    set_category("root Ryu/Build")
    set_description("Compile using unity build")

	after_check(function (option)
		cprint(format("(config) Unity build enabled: ${cyan}%s${clear}", option:value()))
	end)
option_end()

option("ryu-enable-hot-reload")
	add_deps("ryu-build-with-editor")
    set_default(true)
    set_showmenu(true)
    set_description("Enable hot-reload for game code (requires editor)")

	after_check(function (option)
		-- If we are not building with editor, then we also cannot hot-reload
		if not option:dep("ryu-build-with-editor"):enabled() then
            option:enable(false)
        end
		cprint(format("(config) Hot-reload enabled: ${cyan}%s${clear}", option:value()))
	end)
option_end()
