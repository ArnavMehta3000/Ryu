target("RyuTestbench")
	add_rules("win.sdk.application")
	set_default(true)
	set_kind("binary")
	set_group("Ryu/Testing")

	add_includedirs(".")
	add_files("**.cpp")
	add_headerfiles("**.h")

	add_deps("RyuCore")
target_end()
