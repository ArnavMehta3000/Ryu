import("core.project.project")
import("core.base.json")

function parse_ryuplugin(targetname)
	local t = project.target(targetname)
	local plugin_file = path.join(t:scriptdir(), ".ryuplugin")
	
	if not os.exists(plugin_file) then
		raise("[RyuPluginReader] Plugin config file not found: " .. plugin_file)
	end

	cprint(format("${dim green}[RyuPluginReader]${clear} ${dim}Parsing plugin file: ${dim}%s${clear}", plugin_file))
	local plugin_config_data  = json.loadfile(plugin_file)
			
	local plugin_config       = plugin_config_data["Plugin"]
	local plugin_version      = plugin_config["Version"]
	local plugin_name         = plugin_config["Name"]
	local plugin_load_order   = plugin_config["LoadOrder"]
	local plugin_tick_order   = plugin_config["TickOrder"]
	local plugin_render_order = plugin_config["RenderOrder"]
		
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
	elseif plugin_tick_order == "PostUpdate" then
		tick_order = "Ryu::PluginTickOrder::PostUpdate"
	elseif plugin_tick_order == nil or plugin_tick_order == "" or plugin_tick_order == "Default" then
		tick_order = "Ryu::PluginTickOrder::Default"
	end

	local render_order = ""
	if plugin_render_order == "None" then
		render_order = "Ryu::PluginRenderOrder::None"
	elseif plugin_render_order == "PreRender" then
		render_order = "Ryu::PluginRenderOrder::PreRender"
	elseif plugin_render_order == "PostRender" then
		render_order = "Ryu::PluginRenderOrder::PostRender"
	elseif plugin_render_order == nil or plugin_render_order == "" or plugin_render_order == "Default" then
		render_order = "Ryu::PluginRenderOrder::Default"
	end

	local plugin_deps = plugin_config["Dependencies"]

	-- Make the plugin table
	local out           = {}
	out["Name"]         = plugin_name
	out["Version"]      = plugin_version
	out["LoadOrder"]    = load_order
	out["TickOrder"]    = tick_order
	out["RenderOrder"]  = render_order
	out["Group"]        = plugin_config["Group"]
	out["Dependencies"] = plugin_deps
	return out
end

function main()
	-- Do nothing in main
end
