target("RyuEditor")
	add_rules("RadDebug")
	add_rules("win.sdk.application")
	add_packages("imgui")

	set_default(true)
	set_kind("binary")
	set_group("Ryu")

	add_includedirs(".")
	add_files("**.cpp")
	add_headerfiles("**.h")

	add_deps("RyuEngine", { inherit = true })

	on_run(function (target)
		local run_editor = function (t)
			os.exec(target:targetfile())
		end

		if has_config("use-raddbg") then
			local rad_path = get_config("raddbg-path")
			print("Running Rad Debugger")
			os.execv(rad_path, { target:targetfile() })
		else
			os.exec(target:targetfile())
		end
	end)
target_end()
