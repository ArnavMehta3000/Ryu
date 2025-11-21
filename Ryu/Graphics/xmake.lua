target("RyuGraphics")
	set_kind("static")
	set_group("Ryu/Core")

	if has_config("ryu-throw-on-fail-hresult") then
		add_defines("RYU_THROW_ON_FAIL_HRESULT")
	end

	add_deps(
		"RyuGlobals",
		"RyuGame",
		"RyuProfiling",
		"RyuThreading",
		"RyuLogging",
		"RyuConfig",
		"RyuMath"
	)

	if has_config("ryu-build-with-editor") then
		add_defines("RYU_WITH_EDITOR")
		add_packages("imgui", { public = true })
	end

	add_rules("EnumToHeader",
		{
			root = path.join(os.projectdir(), "Ryu", "Enums"),
			files =
			{
				"ShaderType.json",
			},
			force = false
		})

	-- Add shader files
	add_files("./Shaders/**.hlsl")
	add_rules("HLSLShader", { root = "Engine" })

	add_includedirs(".", { public = true })

	add_files("Graphics/*.cpp", { unity_group = "Graphics" })
	add_files("Graphics/Core/**.cpp", { unity_group = "GraphicsCore" })
	add_files("Graphics/Compiler/*.cpp", { unity_group = "GraphicsCompiler" })

	add_headerfiles("**.h")
	add_packages("directx-headers", "directxshadercompiler", { public = true })

	add_links("d3d12", "dxgi", "dxguid", "d3dcompiler", "Advapi32")

	if has_config("ryu-unity-build") then
		add_rules("c++.unity_build")
	end
target_end()
