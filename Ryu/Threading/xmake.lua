target("RyuThreading")
	set_group("Ryu/Core")
	set_kind("static")
	add_includedirs(".", { public = true })
	add_files("**.cpp", { unity_group = "Multithreading" })
	add_headerfiles("**.h")
	add_extrafiles("**.inl")
	add_deps("RyuCommon")

	if has_config("ryu-unity-build") then
		add_rules("c++.unity_build")
	end
target_end()
