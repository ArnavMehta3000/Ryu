-- Generates header files for enum with ToString converstions from JSON file
rule("EnumToHeader")
	on_config(function(target)
		-- Add generated directory as include dir
		local dir = path.join(target:autogendir(), "Generated")
		target:add("includedirs",dir, { public = true })
	end)

	on_prepare(function (target, opt)
		import("core.project.depend")
		import("core.base.task")
		import("utils.progress")
		import("core.base.semver")

		local out_dir = path.join(target:autogendir(), "Generated")
		local root_dir = target:extraconf("rules", "EnumToHeader", "root")
		local force_run = target:extraconf("rules", "EnumToHeader", "force")
		local input_files = target:extraconf("rules", "EnumToHeader", "files")

		local depend_files = {}
		for _, file in ipairs(input_files) do
			local source_file = path.join(root_dir, file)

			if not os.exists(source_file) then
				raise(format("[Xmake::EnumToHeader] Enum JSON file does not exist: %s", source_file))
			end
			-- Add complete file path as dependency
			table.insert(depend_files, source_file)
		end

		local tracked_values =
		{
			semver.new(target:version()):shortstr(),
			target:plat(),
			target:arch(),
			os.host(),
			target:get("mode")
		}

		-- Create dependency on input JSON
		depend.on_changed(function()
			progress.show(opt.progress, "${color.build.target}Generating enum headers for %s", target:name())

			-- Run task with the dependency files
			task.run(
				"ryu-enum-to-header",
				{
					files = depend_files,
					output_dir = out_dir
				}) end,
				{
					dependfile = target:dependfile("versioninfo"),
					files = depend_files,
					dryrun = force_run,
					values = tracked_values
				}
			)
	end)

	on_clean(function (target)
		local depend_file = path.join(target:dependir(), "versioninfo.d")

		os.rm(depend_file)
	end)
rule_end()
