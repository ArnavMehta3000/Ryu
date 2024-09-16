target("RyuCore")
	add_rules("GenConfigs", "CommonPackages")

	set_default(false)
	set_kind("static")
	set_group("Ryu")

	add_includedirs("..", { public = true })
	add_files("**.cpp")
	add_headerfiles("**.h")

	add_links("Gdi32")

	on_run(function (target)
		-- Run the config parser for each toml file in the config directory
		local root_dir = os.projectdir()
		local input_dir = path.join(root_dir, "Config", "**.toml")
		
		for _, filedir in ipairs(os.filedirs(input_dir)) do
			local output_filename = path.basename(filedir) .. ".json"
			local output_dir = path.join(root_dir, "build", "Artifacts")
			local output_file = path.join(output_dir, output_filename)
			
			cprint("Generating config file: %s", output_file)
			os.execv("xmake run -q RyuConfigParser", { filedir, output_file })
		end

	end)
target_end()
