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
