rule("CopyToBuildDir")
	on_buildcmd_file(function (target, batchcmds, sourcefile, opt)
		-- Ensure the build directory exists
		os.mkdir(target:targetdir())

		-- Determine run directory (use targetdir if rundir is not set)
        local run_dir = target:get("rundir") or target:targetdir()

        -- Define allowed root folder names
        local allowed_folders = { "Game", "Engine", "Editor" }
        local source_dir = path.directory(sourcefile)

        -- Find the first allowed folder in the source path
        local relative_dir = nil
        for _, folder in ipairs(allowed_folders) do
            local folder_start = string.find(source_dir, folder .. "[\\/]")
            if folder_start then
                relative_dir = source_dir:sub(folder_start)
                break
            end
        end

        -- Handle case where no allowed folder is found
        if not relative_dir then
            print("Error: None of the allowed folders ('Game', 'Engine', 'Editor') found in path: " .. source_dir)
            return
        end

        -- Construct the output directory
        local output_dir = path.join(target:targetdir(), relative_dir)

		-- Copy the file into the output directory
		local dest_file = path.join(output_dir, path.filename(sourcefile))
		batchcmds:cp(sourcefile, dest_file)
		batchcmds:show_progress(opt.progress, "${color.build.object}Script file updated: %s", sourcefile)

		batchcmds:add_depfiles(sourcefile)
		batchcmds:set_depmtime(os.mtime(dest_file))
		batchcmds:set_depcache(target:dependfile(dest_file))
	end)
rule_end()
