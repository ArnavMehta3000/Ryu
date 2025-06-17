-- Marks the game application to be built as a DLL (used with editor)
rule("RyuGame")
	on_load(function (target)
		if has_config("ryu-game-as-dll") then
			target:add("defines", "RYU_GAME_AS_DLL")
			target:set("kind", "shared")
		else
			target:set("kind", "binary")
			-- Use WinMain only in release mode
			if not is_mode("debug", "releasedbg") then
				target:add("rules", "win.sdk.application")
			end
		end
	end)

	on_config(function (target)
		if has_config("ryu-game-as-dll") then
			-- Add this target as a dependency of the editor
			import("core.project.project")
			local editor = project.target("RyuEditor")
			if editor then
				editor:add("deps", target:name())
			end
		end
	end)
rule_end()
