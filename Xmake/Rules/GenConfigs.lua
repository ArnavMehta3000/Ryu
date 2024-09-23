-- Generate config files
rule("GenConfigs")
	add_deps("IncludeConfigs")

	on_config(function (target)
		target:add("options", "log-level")
		-- Log level
		if has_config("log-level") then
			local level = string.upper(get_config("log-level"))

			if level == "TRACE" then
				target:set("configvar", "HAS_LOG_WARN", 1)
				target:set("configvar", "HAS_LOG_INFO", 1)
				target:set("configvar", "HAS_LOG_DEBUG", 1)
				target:set("configvar", "HAS_LOG_TRACE", 1)
			elseif level == "DEBUG" then
				target:set("configvar", "HAS_LOG_WARN", 1)
				target:set("configvar", "HAS_LOG_INFO", 1)
				target:set("configvar", "HAS_LOG_DEBUG", 1)
				target:set("configvar", "HAS_LOG_TRACE", 0)
			elseif level == "INFO" then
				target:set("configvar", "HAS_LOG_WARN", 1)
				target:set("configvar", "HAS_LOG_INFO", 1)
				target:set("configvar", "HAS_LOG_DEBUG", 0)
				target:set("configvar", "HAS_LOG_TRACE", 0)
			else
				target:set("configvar", "HAS_LOG_WARN", 1)
				target:set("configvar", "HAS_LOG_INFO", 0)
				target:set("configvar", "HAS_LOG_DEBUG", 0)
				target:set("configvar", "HAS_LOG_TRACE", 0)
			end
		else
			local level = is_mode("debug") and "trace" or "info"
			cprint(format("${cyan}%s(log-level)${clear} config not set. Defaulting log verbosity level to: ${green}%s", target:name(), level))

			-- Warning and info is always enabled if config is not set
			target:set("configvar", "HAS_LOG_WARN", 1)
			target:set("configvar", "HAS_LOG_INFO", 1)

			if string.upper(level) == "INFO" then
				target:set("configvar", "HAS_LOG_DEBUG", 0)
				target:set("configvar", "HAS_LOG_TRACE", 0)
			end

			if string.upper(level) == "TRACE" then
				target:set("configvar", "HAS_LOG_DEBUG", 1)
				target:set("configvar", "HAS_LOG_TRACE", 1)
			end
		end

		-- Log console
		if has_config("console") then
			if get_config("console") == true then
				target:set("configvar", "HAS_CONSOLE_LOG", 1)
				cprint("Log console enabled: ${green}yes")
			end
		else
			cprint("Log console enabled: ${red}no")
			target:set("configvar", "HAS_CONSOLE_LOG", 0)
		end

		-- Release mode assertions
		if has_config("enable-assert") then
			if get_config("enable-assert") == true then
				target:set("configvar", "HAS_ASSERTS", 1)
				cprint("Assertions enabled: ${green}yes")
			end
		else
			cprint("Assertions enabled: ${red}no")
			target:set("configvar", "HAS_ASSERTS", 0)
		end

		target:add("configfiles", path.join("$(projectdir)", "Xmake", "Templates", "Core", "*.h.in"))
	end)
rule_end()
