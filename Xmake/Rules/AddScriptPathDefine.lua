rule("AddScriptPathDefine")
	on_config(function (target)
		local define_name = target:extraconf("rules", "AddScriptPathDefine", "name")

		local script_dir = target:scriptdir()
		script_dir = string.gsub(script_dir, "\\", "/")

		local define = define_name .. "=\"" .. script_dir .. "\""
		target:add("defines", define)
	end)
rule_end()
