-- Set include directory for config files and plugins
rule("IncludeConfigs")
	on_config(function (target)
		local config_dir = "$(buildir)/Config"
		target:set("configdir", config_dir)
		target:add("includedirs", config_dir, { public = true })
		target:add("options", "console")
		target:add("options", "enable-assert")

	end)
rule_end()

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

-- Rule to include packages
rule("CommonPackages")
	on_load(function (target)
		target:add("packages", "spdlog")
		target:add("packages", "backward-cpp")
		target:add("packages", "nlohmann_json")
	end)
rule_end()

-- Rule to build target as DLL
rule("BuildAsDLL")
	on_config(function (target)
		target:set("kind", "shared")
		target:set("enabled", true)
		target:add("defines", "RYU_BUILD_DLL")
		target:add("defines", "RYU_EXPORT")
		target:add("cxflags", "/wd4251")  -- Disable C4251 warning: 'X' needs to have dll-interface
	end)
rule_end()

-- Builds as DLL and sets config variables
rule("RyuPlugin")
	set_extensions(".ryuplugin")
	add_deps("BuildAsDLL")

	on_config(function (target)
		target:set("policy", "build.fence", false)	

		-- What is this plugin being built for? (Editor or Engine) -> decides where to put generated files
		local plugin_for = target:extraconf("rules", "RyuPlugin", "built_for")
		if plugin_for == nil then
			raise("Plugin built_for not specified! Valid values: Editor, Engine")
		end
		
		if string.upper(plugin_for) ~= "EDITOR" and string.upper(plugin_for) ~= "ENGINE" then
			raise("Invalid plugin built for specified! Valid values: Editor, Engine")
		end

		cprint(format("${green}[RyuPlugin]${clear} Configuring ${cyan}%s${clear} as ${magenta}%s${clear} plugin", target:name(), plugin_for))

		target:add("files", path.join(target:scriptdir(), ".ryuplugin"))
		local plugin_reader = import("PluginReader")
		local plugin_config = plugin_reader.parse_ryuplugin(target:name())
		local plugin_template = path.join(os.projectdir(), "Xmake", "Templates", "Plugin", "PluginData.h.in")
		local gen_file_name = path.join("Plugins", plugin_for, target:name(), "Generated", "PluginData.h")

		-- Add config files
		target:add("configfiles", plugin_template, { onlycopy = false, filename = gen_file_name })

		local log_template = path.join(os.projectdir(), "Xmake", "Templates", "Plugin", "PluginLog.h.in")
		local log_file_name = path.join("Plugins", plugin_for, target:name(), "Generated", plugin_config["Name"] .. "Log.h")
		target:add("configfiles", log_template, { onlycopy = false, filename = log_file_name })
				
		target:set("configvar", "PLUGIN_VERSION",      plugin_config["Version"])
		target:set("configvar", "PLUGIN_NAME",         plugin_config["Name"])
		target:set("configvar", "PLUGIN_LOAD_ORDER",   plugin_config["LoadOrder"], { quote = false })
		target:set("configvar", "PLUGIN_TICK_ORDER",   plugin_config["TickOrder"], { quote = false })
		target:set("configvar", "PLUGIN_RENDER_ORDER", plugin_config["RenderOrder"], { quote = false })
		
		cprint(format("${dim green}[RyuPlugin]${clear} ${dim}Auto generated plugin files for: %s${clear}", plugin_config["Name"]))

		-- Configure dependencies
		local plugin_deps = plugin_config["Dependencies"]
		for _, dep in ipairs(plugin_deps) do
			local name = dep["Name"]
			local create_link = dep["CreateLink"]
			
			target:add("deps", name)
			
			if create_link == true then
				target:add("links", name)
			end
		end

		-- Set target group
		local group = plugin_config["Group"]
		if group == nil or group == "" then
			target:set("group", "Plugins")
		else
			target:set("group", group)
		end
	end)
rule_end()
