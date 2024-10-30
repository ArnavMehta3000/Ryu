target("RyuGraphics")
	set_kind("static")
	set_group("Ryu/Graphics")

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
