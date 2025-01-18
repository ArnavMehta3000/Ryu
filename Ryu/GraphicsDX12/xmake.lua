target("RyuGraphicsDX12")
	set_enabled(false)
	set_kind("static")
	set_group("Ryu/Graphics")

	if has_config("assert-on-fail-hresult") then
		add_defines("RYU_BREAK_ON_FAIL_HRESULT")
	end

	add_includedirs(".", { public = true })
	add_files("**.cpp")
	add_headerfiles("**.h")

	add_packages("directx-headers", { public = true })

	add_deps("RyuGraphicsRHI")
	add_links("d3d12", "dxgi", "dxguid")
target_end()
