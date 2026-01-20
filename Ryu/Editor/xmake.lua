target("RyuEditor")
    set_default(true)
    set_kind("binary")
    set_group("Ryu")
    set_enabled(get_config("ryu-build-with-editor"))
    
    if not is_mode("debug", "releasedbg") then
        add_rules("win.sdk.application")
    end
    
    add_rules("utils.bin2obj", { extensions = { ".otf", ".ttf" } })
    
    add_includedirs(".")
    add_files("**.cpp", { unity_group = "Editor" })
    add_headerfiles("**.h")
    add_files("Fonts/**.otf", "Fonts/**.ttf")
    
    add_deps("RyuEngine")
    
    add_defines("RYU_IS_EDITOR")
    
    -- Hot-reload support
    if has_config("ryu-enable-hot-reload") then
        add_defines("RYU_HOT_RELOAD")
    end
    
    if has_config("ryu-unity-build") then
        add_rules("c++.unity_build")
    end

    after_build( function(target)
        import("lib.detect.find_tool")
        
        local radbin = find_tool("radbin")
        if not radbin then
			return
		end
        
        local write_dir = os.projectdir()  -- Root  directory for now
        local target_file = target:targetfile();

        -- Call radbin .exe to generate .rdi file from exe


		os.vrunv(radbin.program, {"--rdi", target_file })
    end)
target_end()
