local engine_plugins =
{
	"RyuInput"
}

target("RyuEngine")
	on_config (function (target)
		-- Add engine plugins
		for i, plugin in ipairs(engine_plugins) do
			-- Get plugin directory
			local prj_dir = "$(projectdir)";
			local plugin_dir = path.join(prj_dir, "Plugins", "Plugins", "Engine", plugin)

			-- Ensure plugin directory has xmake.lua file
			local plugin_xmake = path.join(plugin_dir, "xmake.lua")
			if not os.isfile(plugin_xmake) then
				raise("Engine plugin file not found: " .. plugin_xmake)
			end

			-- Add plugin dependency
			target:add("deps", plugin)

			cprintf("Add engine plugin dependency [%u]: ${blue}%s (%s)${clear}\n", i, plugin, plugin_dir)
		end

		-- Add engine config toml
		local config_dir = path.join("$(projectdir)", "Config", "EngineConfig.toml")
		target:add("files", config_dir)
	end)

	-- Embed engine config
	add_rules("utils.bin2c", {extensions = {".toml"}})

	add_rules(
		"IncludeConfigs",
		"CommonPackages",
		"BuildAsDLL")

	set_default(false)
	set_kind("shared")
	set_group("Ryu")

	add_includedirs("..", { public = true })
	add_files("**.cpp")
	add_headerfiles("**.h")

	add_deps("RyuCore")
	add_links("RyuCore")
target_end()