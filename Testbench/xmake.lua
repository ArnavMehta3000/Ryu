target("RyuTestbench")
	-- Use WinMain only in release mode
	if not is_mode("debug", "releasedbg") then
		add_rules("win.sdk.application")
	end

	set_default(true)
	set_kind("binary")
	set_group("Ryu/Testing")

	add_includedirs(".")
	add_files("**.cpp")
	add_headerfiles("**.h")
	add_deps("RyuEngine")
target_end()
