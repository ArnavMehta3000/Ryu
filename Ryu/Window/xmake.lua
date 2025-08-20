target("RyuWindow")
	set_group("Ryu/Core")
	set_kind("static")
	add_includedirs(".", { public = true })
	add_files("**.cpp", { unity_group = "Window" })
	add_headerfiles("**.h")

	add_deps("RyuConfig", "RyuProfiling", "RyuEvent")
	add_links("Dwmapi")

	if has_config("ryu-unity-build") then
		add_rules("c++.unity_build")
	end
target_end()
