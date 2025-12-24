--[[  Offline compilation of HLSL shaders using DXC
How to use options:
- type = array including "VS" | "HS" | "DS" | "GS" | "PS" | "CS"
- refl = true | false -> enable reflection
- rootsig = "" | string to specify root signature define (will also generate a <filename>.rootsig)
- main = "" | string to specify main function name (default is <type>Main (eg. VSMain | PSMain))
--]]
rule("RyuOfflineShader")
	set_extensions(".hlsl", ".hlsli")

	on_build_file(function (target, sourcefile, opt)
		import("lib.detect.find_tool")
		import("core.project.depend")

		local dxc = find_tool("dxc", { check = "--help" })

		if dxc == nil then
			cprint("${bright red}[RyuShader] DXC not found. Failed to compile %s", sourcefile)
			os.raise("DXC compiler not found")
		end

		local config = target:fileconfig(sourcefile)
		local types = config and config.type or {}

		local basename = path.basename(sourcefile)

		-- If this is changed, then ensure the path is changed on the on_clean function as well
		local outputdir = path.join(target:targetdir(), "Shaders", "Compiled")  --

		-- Map shader types to profiles
		local shaderProfiles =
		{
			VS = "vs_6_0",
			HS = "hs_6_0",
			DS = "ds_6_0",
			GS = "gs_6_0",
			PS = "ps_6_0",
			CS = "cs_6_0"
		}

		-- Generate root signature once per file (not per shader type)
		local rootSigGenerated = false

		-- Loop through shader types
		for _, shaderType in ipairs(types) do
			local profile = shaderProfiles[shaderType]
			if not profile then
				cprint("${bright red}[RyuShader] Unknown shader type: %s", shaderType)
				os.raise("Invalid shader type")
			end

			-- Configure entry point
			local entryPoint = (config and config.main) or (shaderType .. "Main")
			local outFileName = basename .. shaderType
			local outputFile = path.join(outputdir, outFileName .. ".cso")

			local dependfile = target:dependfile(path.join("BuildCache", path.filename(sourcefile)))
			local dependFileList = { sourcefile, outputFile}

			-- Create output directories if they don't exist
			os.mkdir(outputdir)

			depend.on_changed(function()
				cprint("${cyan}[RyuShader] Compiling %s (%s)...", basename, shaderType)

				-- Build DXC arguments
				local args =
				{
					"-T", profile,
					"-E", entryPoint,
					"-Fo", outputFile,
					"-WX",  -- Warnings as errors
					-- "-Vd",  -- Disable validation
					"-ftime-report", -- Time report
					"-I", path.join(os.scriptdir(), "Shaders"),
					sourcefile
				}

				if is_mode("debug", "releasedbg") then
					table.insert(args, "-Zi")
					table.insert(args, "-Fd")  -- Write a file with .pdb extension that will contain the debug information
					table.insert(args, path.join(outputdir, outFileName .. ".pdb"))

					table.insert(dependFileList, path.join(outputdir, outFileName .. ".pdb"))
				end

				-- Enable reflection
				if config and config.refl then
					table.insert(args, "-Fre")
					table.insert(args, path.join(outputdir, outFileName .. ".reflect"))

					table.insert(dependFileList, path.join(outputdir, outFileName .. ".reflect"))
				end

				-- Generate root signature only once per file (use base filename without type suffix)
				if config and config.rootsig and not rootSigGenerated then
					table.insert(args, "-rootsig-define")
					table.insert(args, config.rootsig)
					table.insert(args, "-Frs")
					local rootSigFile = path.join(outputdir, basename .. ".rootsig")
					table.insert(args, rootSigFile)

					table.insert(dependFileList, rootSigFile)
					rootSigGenerated = true
				end

				-- Run DXC
				local outdata, errdata = os.iorunv(dxc.program, args)

				if errdata ~= nil and errdata ~= "" then
					cprint("${bright yellow}[RyuShader] Compilation message: %s", sourcefile)
					print(errdata)

					-- Fail if we have an error
					if string.find(errdata, "error") then
						os.raise()
					end
				end
			 end, { dependfile = dependfile, files = dependFileList, values = { config, target:get("mode") }})
		end
	end)

	on_clean(function (target)
	    local outputdir = path.join(target:targetdir(), "Shaders", "Compiled")  -- Same file path as ShaderCompiler.cpp

	    -- Remove compiled shader outputs
	    if os.isdir(outputdir) then
	        os.rm(outputdir)
	        cprint("${yellow}[RyuShader] Cleaned compiled shaders")
	    end

	    -- Remove dependency cache files
	    local buildCache = path.join(target:dependdir(), "BuildCache")
	    if os.isdir(buildCache) then
	        os.rm(buildCache)
	        cprint("${yellow}[RyuShader] Cleaned dependency cache")
	    end
	end)
rule_end()
