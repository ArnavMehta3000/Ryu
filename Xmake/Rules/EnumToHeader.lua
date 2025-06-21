-- Generates header files for enum with ToString converstions from JSON file
rule("EnumToHeader")
	on_config(function(target)
		-- Add generated directory as include dir
		local dir = path.join(target:autogendir(), "Generated")
		target:add("includedirs",dir, { public = true })
	end)

	before_build(function (target)
		import("core.project.depend")
		import("core.base.task")

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

		-- Run task with the dependency files
		depend.on_changed(function()
			task.run("ryu-enum-to-header", { files = depend_files, output_dir = out_dir })
			end, { files = depend_files, dryrun = force_run, values = target:get("mode") }
		)
	end)
rule_end()
