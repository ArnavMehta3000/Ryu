target("RyuGraphicsDX12")
	set_enabled(get_config("ryu-rhi-dx12"))

	set_kind("static")
	set_group("Ryu/Graphics")

	if has_config("assert-on-fail-hresult") then
		add_defines("RYU_BREAK_ON_FAIL_HRESULT")
	end

	add_includedirs(".", { public = true })
	add_files("**.cpp", { unity_group = "GraphicsDX12" })
	add_headerfiles("**.h")

	add_packages("directx-headers", { public = true })

	add_deps("RyuGraphicsRHI")
	add_links("d3d12", "dxgi", "dxguid")
	add_rules("c++.unity_build")
target_end()
