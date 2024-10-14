target("RyuGraphics")
	set_kind("static")
	set_group("Ryu/Graphics")

	add_includedirs(".", { public = true })
	add_files("**.cpp")
	add_headerfiles("**.h")

	add_deps("RyuCore")
	add_links("d3d12", "dxgi", "dxguid")
target_end()
