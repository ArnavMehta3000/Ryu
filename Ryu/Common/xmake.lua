target("RyuCommon")
	set_group("Ryu/Core")
	set_kind("headeronly")

	add_includedirs(".", { public = true })
	add_headerfiles("**.h")
	--add_files("**.cpp", { unity_group = "Common" })
target_end()
