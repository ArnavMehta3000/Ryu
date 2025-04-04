target("RyuGraphics")
	set_kind("static")
	set_group("Ryu/Graphics")

	if has_config("assert-on-fail-hresult") then
		add_defines("RYU_BREAK_ON_FAIL_HRESULT")
	end

	add_includedirs(".", { public = true })
	add_files("**.cpp", { unity_group = "Graphics" })
	add_headerfiles("**.h")

	add_deps(
		"RyuGraphicsRHI",
		"RyuGraphicsDX11",
		"RyuGraphicsDX12"
	)
	add_rules("c++.unity_build")
target_end()
