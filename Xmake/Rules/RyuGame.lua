rule("RyuGame")
    on_load(function(target)
        local with_editor = has_config("ryu-build-with-editor")
        local hot_reload = has_config("ryu-enable-hot-reload")
        
        if with_editor and hot_reload then
            -- DLL for hot-reload in editor
            target:set("kind", "shared")
            target:add("defines", "RYU_WITH_EDITOR", "RYU_HOT_RELOAD", "RYU_GAME_DLL")
        elseif with_editor then
            -- Static link to editor (no hot-reload)
            target:set("kind", "object")
            target:add("defines", "RYU_WITH_EDITOR")
        else
            -- Standalone executable
            target:set("kind", "binary")
            if not is_mode("debug", "releasedbg") then
                target:add("rules", "win.sdk.application")
            end
        end
        
        target:add("extrafiles", "Config/**.toml")
    end)
    
    on_config(function(target)
        import("core.project.project")
        
        local with_editor = has_config("ryu-build-with-editor")
        local hot_reload = has_config("ryu-enable-hot-reload")
        
        if with_editor then
            local editor = project.target("RyuEditor")
            if editor then
                if hot_reload then
                    -- Don't link, just tell editor where to find DLL
                    local project_root = os.projectdir()
                    local output_dir = target:targetdir()
                    local dll_name = target:filename()
                    editor:add("runargs", "--Game.ModulePath=" .. path.join(project_root, output_dir, dll_name))

                else
                    -- Static link
                    editor:add("deps", target:name())
                end
                
                local game_root = target:scriptdir()
                editor:add("runargs", "--App.ProjectDir=" .. game_root)

                -- NOTE: Adding this for now so that testbench is built along with the editor
				--editor:add("deps", target:name())
            end
        end
    end)
rule_end()

