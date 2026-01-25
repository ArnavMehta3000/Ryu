--[[  Offline compilation of HLSL shaders using DXC
How to use options:
- type    = array including "VS" | "HS" | "DS" | "GS" | "PS" | "CS"
- refl    = [true | false] -> enable reflection
- rootsig = ["" | string] to specify root signature define (will also generate a <filename>.rootsig)
- main    = ["" | string] to specify main function name (default is <type>Main (eg. VSMain | PSMain))
--]]

rule("RyuOfflineShader")
	on_load(function (target)
		-- Create output directory
		local outputdir = path.join(target:targetdir(), "Shaders", "Compiled")
		if not os.isdir(outputdir) then
			os.mkdir(outputdir)
		end
	end)

	before_buildcmd_file(function (target, batchcmds, source_hlsl, opt)
		import("lib.detect.find_tool")

		-- Locate DXC compiler (use hardcoded path)
		local DXC_PATH = path.join(os.projectdir(), "External", "DXC", "bin", "x64")
		local dxc = find_tool("dxc", { check = "--help", paths = { DXC_PATH } })
		assert(dxc, "DXC not found")

		-- Final output folder
		local outputdir = path.join(target:targetdir(), "Shaders", "Compiled")

		local basename = path.basename(source_hlsl)
		local config   = target:fileconfig(source_hlsl)
		local types    = config and config.type or {}  -- Types of shader profiles to compile for

		-- Map shader types to profiles (using SM6)
		local shaderProfiles =
		{
			VS = "vs_6_0",
			HS = "hs_6_0",
			DS = "ds_6_0",
			GS = "gs_6_0",
			PS = "ps_6_0",
			CS = "cs_6_0"
		}

		batchcmds:add_depfiles(source_hlsl)

		-- Generate root signature once per file (not per shader type)
		local rootSigGenerated = false

		-- Loop through the shader types for this file
		for _, shadertype in ipairs(types) do
			-- Get profile from type
			local profile = shaderProfiles[shadertype]
			assert(profile, "Invalid shader type!")

			-- Configure entry point (either user provided or <Type>Main)
			local entrypoint = (config and config.main) or (shadertype .. "Main")

			-- Get output filename/dir
			local outfilename = basename .. shadertype
			local outputfile = path.join(outputdir, outfilename .. ".cso")

			-- Build dxc args
			local args =
			{
				"-T", profile,
				"-E", entrypoint,
				"-Fo", outputfile,
				"WX", -- Warnings as errors
				"-I", path.join(target:scriptdir(), "Shaders")
			}

			if is_mode("debug", "releasedbg") then
				table.insert(args, "-Zi")
				table.insert(args, "-Fd")  -- Write a file with .pdb extension that will contain the debug information
				table.insert(args, path.join(outputdir, outfilename .. ".pdb"))
			end

			-- Enable reflection
			if config and config.refl then
				table.insert(args, "-Fre")
				table.insert(args, path.join(outputdir, outfilename .. ".reflect"))
			end

			-- Generate root signature only once per file (use base filename without type suffix)
			if config and config.rootsig and not rootSigGenerated then
				table.insert(args, "-rootsig-define")
				table.insert(args, config.rootsig)
				table.insert(args, "-Frs")
				table.insert(args, path.join(outputdir, basename .. ".rootsig"))
				rootSigGenerated = true
			end

			table.insert(args, source_hlsl)

			batchcmds:show_progress(opt.progress, "${color.build.object}compiling.hlsl(%s) %s", shadertype, source_hlsl)
			batchcmds:mkdir(outputdir)
			batchcmds:vrunv(dxc.program, args)

			batchcmds:set_depmtime(os.mtime(outputfile))
			batchcmds:set_depcache(target:dependfile(outputfile))
		end
	end)

	after_clean(function (target, batchcmds, source_hlsl)
		local outputdir = path.join(target:targetdir(), "Shaders", "Compiled")
		
		-- Remove the entire compiled directory
		os.rm(outputdir, {async = true, detach = true})
		
	end)
rule_end()
