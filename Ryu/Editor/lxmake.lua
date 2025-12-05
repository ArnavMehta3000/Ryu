target("RyuEditor")
	set_default(true)
	set_kind("binary")
	set_group("Ryu")
	set_enabled(get_config("ryu-build-with-editor"))

	if not is_mode("debug", "releasedbg") then
		add_rules("win.sdk.application")
	end

	add_includedirs(".")
	add_files("**.cpp", { unity_group = "Editor" })
	add_headerfiles("**.h")

	add_deps("RyuEngine")

	if has_config("ryu-unity-build") then
		add_rules("c++.unity_build")
	end

	add_defines("RYU_IS_EDITOR")
target_end()
