target("RyuGraphicsDX11")
	set_enabled(get_config("ryu-rhi-dx11"))
	set_kind("static")
	set_group("Ryu/Graphics")

	if has_config("ryu-assert-on-fail-hresult") then
		add_defines("RYU_BREAK_ON_FAIL_HRESULT")
	end

	add_includedirs(".", { public = true })
	add_files("**.cpp", { unity_group = "GraphicsDX11" })
	add_headerfiles("**.h")

	add_deps("RyuGraphicsRHI")
	add_links("d3d11", "dxgi", "dxguid")
	add_rules("c++.unity_build")
target_end()
