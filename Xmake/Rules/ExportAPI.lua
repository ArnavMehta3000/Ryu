-- Adds the RYU_EXPORTS macro if the target is a shared library
rule("ExportAPI")
	on_load(function (target)
		if target:is_shared() then
			target:add("defines", "RYU_EXPORTS")
		end
	end)
rule_end()
