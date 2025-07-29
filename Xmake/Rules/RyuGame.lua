-- Marks the game application to be built as a DLL (used with editor)
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
		-- Add this target as a dependency of the editor
		if has_config("ryu-build-with-editor") then
			import("core.project.project")
			local editor = project.target("RyuEditor")
			if editor then
				editor:add("deps", target:name())
			end
		end
	end)
rule_end()
