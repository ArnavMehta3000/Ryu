target("RyuEditor")
	add_rules("win.sdk.application")
	set_default(true)
	set_kind("binary")
	set_group("Ryu")

	add_includedirs(".")
	add_files("**.cpp", { unity_group = "Editor" })
	add_headerfiles("**.h")

	add_packages("imgui")
	add_deps("RyuEngine")
	add_rules("c++.unity_build")
target_end()
