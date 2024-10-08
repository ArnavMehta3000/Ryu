-- Defines an option to set the path of the Rad Debugger locally
option("raddbg-path")
	set_description("Path to the Rad Debugger")
	set_showmenu(true)
	set_category("root Ryu/Debugging")
option_end()

-- Defines an option that allows the target to use the Rad debugger
option("use-raddbg")
	set_description("Use Rad Debugger when using xmake run command")
	add_deps("raddbg-path")
	set_showmenu(true)
	set_category("root Ryu/Debugging")
	set_default(false)

	after_check(function (option)
		if option:enabled() then
			local value = option:dep("raddbg-path"):value()
			if type(value) ~= "string" then
				option:enable(false)
				raise("Cannot enable 'use-raddbg' if option 'raddbg-path' is empty!")
			else
				if not os.isexec(value) then
					raise("Rad Debugger path is invalid! Config 'raddbg': " .. tostring(value))
				end
			end
		end
	end)
option_end()
