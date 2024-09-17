target("RyuEngine")
	on_config (function (target)
		import("core.base.json")
		
		-- Load engine config
		local config_file = path.join(os.projectdir(), "Config", "EngineConfig.json")		
		if not os.exists(config_file) then
			raise("Engine config file not found: " .. config_file)
		end

		-- Load engine config JSON
		local engine_config  = json.loadfile(config_file)
		local plugins_config = engine_config["PluginsConfig"]
		local plugins_path   = plugins_config["PluginsPath"]
		local plugins        = plugins_config["Plugins"]

		-- Add engine plugins
		for i, plugin_name in ipairs(plugins) do
			local plugin_dir = path.join(plugins_path, plugin_name)
			plugin_dir = path.absolute(plugin_dir, os.projectdir())

			-- Ensure plugin is valid (check for '.ryuplugin' file)
			local ryu_plugin_file = path.join(plugin_dir, ".ryuplugin")
			if not os.exists(ryu_plugin_file) then
				raise("Plugin file not found: " .. ryu_plugin_file)
			end

			-- Add target dependency
			target:add("deps", plugin_name)

			-- Add include directory
			target:add("includedirs", plugin_dir)

			cprint(format("${green}[Engine]${clear} Using plugin [%u] - ${cyan}%s${clear}", i, plugin_name))
		end

		-- Add engine config file to target
		target:add("files", config_file)
	end)
	
	add_rules("utils.bin2c", {extensions = {".json"}})

	add_rules(
		"IncludeConfigs",
		"CommonPackages",
		"BuildAsDLL")

	set_default(false)
	set_group("Ryu")

	add_includedirs("..", { public = true })
	add_files("**.cpp")
	add_headerfiles("**.h")

	add_deps("RyuCore")
	add_links("RyuCore")
target_end()
