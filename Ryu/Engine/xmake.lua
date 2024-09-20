target("RyuEngine")
	add_rules("utils.bin2c", { extensions = { ".json" } })

	add_rules(
		"IncludeConfigs",
		"CommonPackages",
		"BuildAsDLL")


	on_config(function (target)
		import("core.base.json")
		
		-- Load engine config
		local config_file = path.join(os.projectdir(), "Config", "EngineConfig.json")		
		if not os.exists(config_file) then
			raise("[RyuEngine] Engine config file not found: " .. config_file)
		end

		-- Load engine config JSON
		local engine_config  = json.loadfile(config_file)
		local plugins_config = engine_config["PluginsConfig"]
		local plugins_path   = plugins_config["PluginsPath"]
		local plugins        = plugins_config["Plugins"]

		-- On config may be call multiple times, so we need to remove duplicates (Engine plugins)
		function add_unique(tbl, value)
			if not tbl.seen then
				tbl.seen = {}
			end

			if not tbl.seen[value] then
				table.insert(tbl, value)
				tbl.seen[value] = true
			end
		end

		-- Add engine plugins
		for i, plugin_name in ipairs(plugins) do
			local plugin_dir = path.join(plugins_path, plugin_name)
			plugin_dir = path.absolute(plugin_dir, os.projectdir())

			-- Ensure plugin is valid (check for '.ryuplugin' file)
			local ryu_plugin_file = path.join(plugin_dir, ".ryuplugin")
			if not os.exists(ryu_plugin_file) then
				raise("[RyuEngine] Plugin file not found: " .. ryu_plugin_file)
			end

			-- Add target dependency
			target:add("deps", plugin_name)

			-- Add include directory
			target:add("includedirs", plugin_dir)

			cprint(format("${green}[RyuEngine]${clear} Using plugin [%u] - ${cyan}%s${clear}", i, plugin_name))
		end

		-- Add engine config file to target
		target:add("files", config_file)
	end)

	set_default(false)
	set_group("Ryu")

	add_includedirs("..", { public = true })
	add_files("**.cpp")
	add_headerfiles("**.h")

	add_deps("RyuCore")
	add_links("RyuCore")
target_end()
