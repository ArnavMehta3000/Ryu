-- Copies shader files to the output directory
rule("HLSLShader")
	set_extensions(".hlsl")
	on_build_file(function (target, sourcefile, opt)
	 	import("core.project.depend")
        import("utils.progress")

		local root = target:extraconf("rules", "HLSLShader", "root")
        local shader_output_dir = path.join(target:targetdir(), "Shaders", root)

        -- Split from 'Shaders' directory onwards
        local shaders_pos = sourcefile:find("Shaders[/\\]")
        local relative_path = sourcefile:sub(shaders_pos + 8) -- Skip "Shaders/"

        local dest_file = path.join(shader_output_dir, relative_path)

        depend.on_changed(function ()
        	progress.show(opt.progress, "${color.build.target}Copying shader %s", sourcefile)
	        os.cp(sourcefile, dest_file)
        end,
        {
        	dependfile = target:dependfile(path.join("ShaderCopyCache", path.filename(sourcefile))),
        	files = { sourcefile, dest_file }
        })
	end)

	on_clean(function (target)
		local shaders_dir = path.join(target:targetdir(), "Shaders")
		local comp_shaders_dir = path.join(target:targetdir(), "Compiled")
		local depend_file = path.join(target:dependir(), "ShaderCopyCache")

		os.rm(shaders_dir)
		os.rm(comp_shaders_dir)
		os.rm(depend_file)
	end)
rule_end()
