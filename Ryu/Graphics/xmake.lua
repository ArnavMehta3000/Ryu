target("RyuGraphics")
	set_kind("static")
	set_group("Ryu/Core")

	if has_config("ryu-throw-on-fail-hresult") then
		add_defines("RYU_THROW_ON_FAIL_HRESULT")
	end

	add_deps(
		"RyuGlobals",
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
				"CommandListFLags.json",
				"CommandListType.json",
				"CommandQueuePriority.json",
				"DescriptorHeapFlags.json",
				"DescriptorHeapType.json",
				"FenceFlag.json",
				"Format.json",
				"TextureFlags.json",
				"TextureType.json",
			},
			force = false
		})

	-- Add shader files
	add_files("./Shaders/**.hlsl")
	add_rules("HLSLShader", { root = "Engine" })

	add_includedirs(".", { public = true })
	add_files("**.cpp", { unity_group = "Graphics" })
	add_headerfiles("**.h")
	add_packages("directx-headers", "directxshadercompiler", { public = true })

	add_links("d3d12", "dxgi", "dxguid", "d3dcompiler", "Advapi32")

	if has_config("ryu-unity-build") then
		add_rules("c++.unity_build")
	end
target_end()
