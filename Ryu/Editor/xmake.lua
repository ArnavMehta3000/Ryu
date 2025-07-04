target("RyuEditor")
	set_enabled(get_config("ryu-build-with-editor"))

	if not is_mode("debug", "releasedbg") then
		add_rules("win.sdk.application")
	end

	set_default(true)
	set_kind("binary")
	set_group("Ryu")

	add_includedirs(".")
	add_files("**.cpp", { unity_group = "Editor" })
	add_headerfiles("**.h")

	add_packages("imgui")
	add_deps("RyuEngine", "RyuPlugin")

	if has_config("ryu-unity-build") then
		add_rules("c++.unity_build")
	end
target_end()
