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
