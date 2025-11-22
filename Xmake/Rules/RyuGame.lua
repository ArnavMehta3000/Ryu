-- Marks the game application to be built as a library (used with editor)
rule("RyuGame")
	on_load(function (target)
		if has_config("ryu-build-with-editor") then
			target:add("defines", "RYU_WITH_EDITOR")
			target:set("kind", "static")
		else
			target:set("kind", "binary")
			-- Use WinMain only in release mode
			if not is_mode("debug", "releasedbg") then
				target:add("rules", "win.sdk.application")
			end
		end

		-- Add TOML config files
		target:add("extrafiles", "Config/**.toml")
	end)

	on_config(function (target)
		import("core.project.project")

		-- Add this target as a dependency of the editor
		if has_config("ryu-build-with-editor") then
			local editor = project.target("RyuEditor")
			if editor then
				editor:add("deps", target:name())

				-- Append the working directory to the run arguments of the editor
				local game_root_dir = target:scriptdir()
				local runargs = editor:get("runargs")
				table.insert(runargs, "--App.ProjectRootDir="..game_root_dir)

				editor:set("runargs", runargs)
			end
		end
	end)
rule_end()
