task("ryu-run-tracy")
	set_category("Ryu's Task")
	set_menu(
	{
		usage = "xmake ryu-run-tracy",
		description = "Download and run the tracy profiler (build/Programs/Tracy) "
	})

	on_run(function()
		import("net.http")
		import("utils.archive")

		local zip_path = path.join(os.projectdir(), "build/Programs/tracy.zip")
		local extract_path = path.join(os.projectdir(), "build/Programs/Tracy")
		local profiler_path = path.join(extract_path, "tracy-profiler.exe")


		-- Download if executable or zip are not found
		if not os.isexec(profiler_path) or not os.isfile(zip_path) then
		    cprint("${cyan}Downloading tracy profiler v0.11.1${clear}")
			-- Download the github release
			http.download("https://github.com/wolfpld/tracy/releases/download/v0.11.1/windows-0.11.1.zip", zip_path)
		else
			-- Profiler already exists, run and exit
		    cprint("${green}Running tracy profiler v0.11.1${clear}")
			os.run(profiler_path)
			return
		end

		-- Ensure zip downloaded
		if os.isfile(zip_path) then
		    cprint("${cyan}Extracting tracy profiler v0.11.1 archive${clear}")
			-- Extract the release
			archive.extract(zip_path, extract_path)

			-- Delete the zip
			os.rm(zip_path)
		else
			raise("Failed to extract or download tracy profiler")
		end

		-- Run the profiler
		if os.isexec(profiler_path) then
		    cprint("${green}Running tracy profiler v0.11.1${clear}")
			os.run(profiler_path)
		else
			raise("Failed to run " .. profiler_path)
		end
	end)
task_end()