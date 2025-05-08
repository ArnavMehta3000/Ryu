target("RyuGraphicsRHI")
	set_kind("static")
	set_group("Ryu/Graphics")

	if has_config("ryu-assert-on-fail-hresult") then
		add_defines("RYU_BREAK_ON_FAIL_HRESULT")
	end

	add_rules("c++.unity_build")
	add_includedirs(".", { public = true })
	add_files("**.cpp")
	add_headerfiles("**.h")

	add_deps(
		"RyuUtils",
		"RyuConfig",
		"RyuLogger",
		"RyuProfiling",
	{ public = true })

	add_links("d3d11", "d3d12", "dxgi", "dxguid")
target_end()
