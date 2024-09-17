-- Set include directory for config files and plugins
rule("IncludeConfigs")
	on_config(function (target)
		local config_dir = "$(buildir)/Config"
		target:add("includedirs", config_dir, { public = true })
		target:set("configdir", config_dir)
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
			cprint(format("${cyan}%s log verbosity configured to: ${green}%s${clear}", target:name(), get_config("log-level")))

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

		target:add("configfiles", path.join("$(projectdir)", "Config", "Templates", "Core", "*.h.in"))
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
		target:add("defines", "RYU_BUILD_DLL")
		target:add("defines", "RYU_EXPORT")
		target:add("cxflags", "/wd4251")  -- Disable C4251 warning: 'X' needs to have dll-interface
	end)
rule_end()

-- Builds as DLL and sets config variables
rule("RyuPlugin")
	add_deps("BuildAsDLL")

	on_load(function (target)
		import("core.base.json")
		local plugin_dir = target:scriptdir()
		local plugin_file = path.join(plugin_dir, ".ryuplugin")
		if not os.exists(plugin_file) then
			raise("Plugin config file not found: " .. plugin_file)
		end

		local plugin_config_data = json.loadfile(plugin_file)
			
		local plugin_config       = plugin_config_data["Plugin"]
		local plugin_version      = plugin_config["Version"]
		local plugin_name         = plugin_config["Name"]
		local plugin_load_order   = plugin_config["LoadOrder"]
		local plugin_tick_order   = plugin_config["TickOrder"]
		local plugin_render_order = plugin_config["RenderOrder"]
		
		target:set("configvar", "PLUGIN_VERSION", plugin_version)
		target:set("configvar", "PLUGIN_NAME", plugin_name)

		local load_order = ""
		if plugin_load_order == "PreInit" then
			load_order = "Ryu::PluginLoadOrder::PreInit"
		elseif plugin_load_order == nil or plugin_load_order == "Default" or plugin_load_order == "PostInit" then
			load_order = "Ryu::PluginLoadOrder::Default"
		end

		local tick_order = ""
		if plugin_tick_order == "None" then
			tick_order = "Ryu::PluginTickOrder::None"
		elseif plugin_tick_order == "PreUpdate" then
			tick_order = "Ryu::PluginTickOrder::PreUpdate"
		elseif plugin_tick_order == nil or plugin_tick_order == "Default" or plugin_tick_order == "PostUpdate" then
			tick_order = "Ryu::PluginTickOrder::Default"
		end

		local render_order = ""
		if plugin_render_order == "None" then
			render_order = "Ryu::PluginRenderOrder::None"
		elseif plugin_render_order == "PreRender" then
			render_order = "Ryu::PluginRenderOrder::PreRender"
		elseif plugin_render_order == nil or plugin_render_order == "Default" or plugin_render_order == "PreRender" then
			render_order = "Ryu::PluginRenderOrder::Default"
		end

		target:set("configvar", "PLUGIN_LOAD_ORDER", load_order, { quote = false })
		target:set("configvar", "PLUGIN_TICK_ORDER", tick_order, { quote = false })
		target:set("configvar", "PLUGIN_RENDER_ORDER", render_order, { quote = false })

		target:add("configfiles", path.join("$(projectdir)", "Config", "Templates", "Plugin", "*.h.in"))


		-- Configure dependencies
		local plugin_deps = plugin_config["Dependencies"]
		for _, dep in ipairs(plugin_deps) do
			local name = dep["Name"]
			local create_link = dep["CreateLink"]
			
			target:add("deps", name)
			
			if create_link == true then
				target:add("links")
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
