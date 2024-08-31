local engine_plugins =
{
	"RyuInput"
}

target("RyuEngine")
	on_config (function (target)
		-- Add engine plugins
		for i, plugin in ipairs(engine_plugins) do
			cprintf("Add engine plugin dependency [%u]: ${blue}%s${clear}\n", i, plugin)
			target:add("deps", plugin)
		end
	end)

	add_rules("IncludeConfigs", "CommonPackages", "BuildAsDLL")

	set_default(false)
	set_kind("shared")
	set_group("Ryu")

	add_includedirs("..", { public = true })
	add_files("**.cpp")
	add_headerfiles("**.h")

	add_deps("RyuCore")
	add_links("RyuCore")
target_end()