target("RyuGraphics")
	set_kind("static")
	set_group("Ryu/Graphics")

	if has_config("ryu-throw-on-fail-hresult") then
		add_defines("RYU_THROW_ON_FAIL_HRESULT")
	end

	add_deps(
		"RyuProfiling",
		"RyuLogger",
		"RyuConfig",
		"RyuMath"
	)

	add_rules("EnumToHeader",
		{
			root = path.join(os.projectdir(), "Ryu", "Enums"),
			files =
			{
				"CmdListFLags.json",
				"CmdListType.json",
				"CmdQueuePriority.json",
				"DescHeapFlags.json",
				"DescHeapType.json",
				"FenceFlag.json",
				"Format.json",
				"TextureFlags.json",
				"TextureType.json",
			},
			force = false
		})

	add_includedirs(".", { public = true })
	add_files("**.cpp", { unity_group = "Graphics" })
	add_headerfiles("**.h")
	add_rules("c++.unity_build")
	add_packages("directx-headers", { public = true })

	add_links("d3d12", "dxgi", "dxguid")
target_end()
