target("RyuEditor")
	add_rules("win.sdk.application")
	add_rules("AddScriptPathDefine", { name = "RYU_EDITOR_DIR" })
	set_default(true)
	set_kind("binary")
	set_group("Ryu")

	add_includedirs(".")
	add_files("**.cpp")
	add_headerfiles("**.h")

	add_packages("imgui")
	add_deps("RyuEngine")
target_end()
