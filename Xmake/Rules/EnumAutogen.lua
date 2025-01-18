rule("EnumAutogen")
	set_extensions(".h")

	on_config(function (target)
		local autogendir = target:autogendir()
		if os.exists(autogendir) then
			target:add("includedirs", autogendir, { public = true })
		end
	end)

	before_build(function (target)
		local generator = import("EnumAutoGenerator")
		generator.process_target(target:name())
	end)
rule_end()
