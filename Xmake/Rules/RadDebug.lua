rule("RadDebug")
	on_load(function (target)
		target:add("options", "use-raddbg")
	end)
rule_end()