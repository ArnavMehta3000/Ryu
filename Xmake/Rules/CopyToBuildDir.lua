rule("CopyToBuildDir")
	on_buildcmd_file(function (target, batchcmds, sourcefile, opt)
		-- Ensure the build directory exists
		os.mkdir(target:targetdir())

		-- Get the relative directory
		local run_dir      = target:get("rundir") or os.projectdir()  -- Default to project directory if rundir is not set
		local relative_dir = path.relative(path.directory(sourcefile), run_dir)
		local output_dir   = path.join(target:targetdir(), relative_dir)

		-- Copy the file into the output directory
		local dest_file = path.join(output_dir, path.filename(sourcefile))
		batchcmds:cp(sourcefile, dest_file)
		batchcmds:show_progress(opt.progress, "${color.build.object}Script file updated: %s", sourcefile)

		batchcmds:add_depfiles(sourcefile)
		batchcmds:set_depmtime(os.mtime(dest_file))
		batchcmds:set_depcache(target:dependfile(dest_file))
	end)
rule_end()
