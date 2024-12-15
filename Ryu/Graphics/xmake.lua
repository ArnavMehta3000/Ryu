target("RyuGraphics")
	set_kind("static")
	set_group("Ryu")

	if has_config("assert-on-fail-hresult") then
		add_defines("RYU_BREAK_ON_FAIL_HRESULT")
	end

	add_includedirs(".", { public = true })
	add_files("**.cpp")
	add_headerfiles("**.h")

	add_packages("directx-headers", { public = true })

	add_deps(
		"RyuUtils",
		"RyuConfig",
		"RyuEvents",
		"RyuLogger",
		"RyuApp"
	)
	add_links("d3d11", "d3d12", "dxgi", "dxguid")
target_end()

option("assert-on-fail-hresult")
	set_showmenu(true)
	set_description("Assert on failure of HRESULT")
	set_category("root Ryu/Graphics")
option_end()
