-- Builds as DLL and sets config variables
rule("RyuPlugin")
	set_extensions(".ryuplugin")
	add_deps("BuildAsDLL")

	on_config(function (target)
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

			if name == "RyuEngine" then
				raise(format("[RyuPlugin] \'RyuEngine\' cannot be a dependency of a plugin! Remove it from the plugin (%s) config file.", plugin_config["Name"]))
			end

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
