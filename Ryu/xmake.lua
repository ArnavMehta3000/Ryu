-- Check if we want to do a unity build for all targets
if has_config("ryu-unity-build") then
    add_rules("c++.unity_build")
end

-------------------- Core Module --------------------
target("RyuCore")
	set_group("Ryu/Objects")
	set_kind("object")

	add_includedirs(".", { public = true })
	add_files("Core/Config//**.cpp", { unity_group = "Config" })
	add_files("Core/Globals/**.cpp", { unity_group = "Globals" })
	add_files("Core/Logging/**.cpp", { unity_group = "Logging" })
	add_files("Core/Utils/**.cpp", { unity_group = "Utilities" })
	add_headerfiles("Core/**.h", { public = true })

	-- Precompiled header
	set_pcxxheader("Core/RyuPCH.h")
	add_forceincludes("Core/RyuPCH.h", { public = true })

	add_packages(
		"spdlog", "uuid_v4", "Elos",
	 	"toml++", "cli11", "efsw", { public = true })

    add_options("ryu-log-level", "ryu-enable-tracy-profiling", { public = true })

    add_rules("EnumToHeader", {
		root = path.join(os.projectdir(), "Ryu", "Enums"),
		files = { "ServiceErrorType.json" },
		force = false
	})

	if get_config("ryu-enable-tracy-profiling") then
		add_packages("tracy", { public = true })
	end
target_end()

-------------------- Application Module --------------------
target("RyuApplication")
	set_group("Ryu/Objects")
	set_kind("object")

	add_files("Application/App/**.cpp", { unity_group = "App" })
	add_files("Application/Event/**.cpp", { unity_group = "Event" })
	add_files("Application/Window/**.cpp", { unity_group = "Window" })
	add_headerfiles("Application/**.h", { public = true })

	add_deps("RyuCore", "ImGui")

    add_options("ryu-log-level")
    add_options("ryu-enable-tracy-profiling", { public = true })
target_end()

-------------------- Math Module --------------------
target("RyuMath")
	set_kind('object')
	set_group("Ryu/Objects")

	add_files("Math/**.cpp", { unity_group = "Math" })
	add_headerfiles("Math/**.h", { public = true })

	add_deps("RyuCore", "SimpleMath", { public = true} )
target_end()

-------------------- Threading Module --------------------
target("RyuThreading")
	set_kind('object')
	set_group("Ryu/Objects")

	add_files("Threading/**.cpp", { unity_group = "Threading" })
	add_headerfiles("Threading/**.h", { public = true })

	add_deps("RyuCore")
target_end()

-------------------- Memory Module --------------------
target("RyuMemory")
	set_kind('object')
	set_group("Ryu/Objects")

	add_files("Memory/**.cpp|Tests/**cpp", { unity_group = "Memory" })  -- Ignore tests
	add_headerfiles("Memory/**.h", { public = true })

	add_deps("RyuCore")

	-- Tests
	for _, testfile in ipairs(os.files("Memory/Tests/*.cpp")) do
		 add_tests(path.basename(testfile),
		 {
			 kind           = "binary",
			 group          = "memory",
			 files          = testfile,
			 languages      = "cxx23",
			 packages       = "doctest",
		 })
	end
target_end()

-------------------- Asset module --------------------
target("RyuAsset")
	set_kind('object')
	set_group("Ryu/Objects")

	add_files("Asset/**.cpp", { unity_group = "Asset" })
	add_headerfiles("Asset/**.h", "Asset/**.inl", { public = true })

	add_deps("RyuCore", "STB", "TinyOBJ", { public = true })
	add_packages("directx-headers", "directxshadercompiler", { public = true })

target_end()

-------------------- Shaders Module --------------------
target("RyuShaders")
	set_kind('object')
	set_group("Ryu/Objects")
	add_files("Shaders/**.hlsl", { rule = "RyuOfflineShader", type = { "VS", "PS" }, refl = true, rootsig = "RootSig" })
target_end()

-------------------- Graphics Module --------------------
target("RyuGraphics")
	set_kind('object')
	set_group("Ryu/Objects")

	add_files("Graphics/*.cpp", { unity_group = "Graphics" })
	add_files("Graphics/Core/**.cpp", { unity_group = "GraphicsCore" })
	add_files("Graphics/Shader/*.cpp", { unity_group = "GraphicsShader" })
	add_files("Graphics/Compiler/*.cpp", { unity_group = "GraphicsCompiler" })
	add_headerfiles("Graphics/**.h", "Graphics/**.inl", { public = true })

	-- Old HLSL shder rule
	-- add_files("Graphics/Shaders/**.hlsl")
	-- add_rules("HLSLShader", { root = "Engine" })

	add_deps("RyuCore", "RyuMath", "RyuShaders", "ImGui")
	add_packages("entt", "directx-headers", "directxshadercompiler", { public = true })

	add_rules("EnumToHeader", {
		root = path.join(os.projectdir(), "Ryu", "Enums"),
		files = { "ShaderType.json" },
		force = false
	})
target_end()

-------------------- Game Module --------------------
target("RyuGame")
	set_kind('object')
	set_group("Ryu/Objects")

	add_files("Game/World/**.cpp", { unity_group = "GameWorld" })
	add_files("Game/Components/**.cpp", { unity_group = "GameComponents" })
	add_headerfiles("Game/**.h", "Game/**.inl", { public = true })

	add_deps("RyuCore", "RyuMath")
	add_packages("entt", "directx-headers", { public = true })
target_end()

-- Engine (also the module where the final linking step takes place)
target("RyuEngine")
	set_kind("static")
	set_group("Ryu")

	add_files("Engine/**.cpp", { unity_group = "Engine" })
	add_headerfiles("Engine/**.h", { public = true })

	add_deps(
		"RyuCore",
		"RyuApplication",
		"RyuMath",
		"RyuGame",
		"RyuAsset",
		"RyuThreading",
		"RyuMemory",
		"RyuGraphics",
		{ public = true }
	)

	add_links(
		"user32",
		"Dwmapi",
		"d3d12",
		"dxgi",
		"dxguid",
		"d3dcompiler",
		"Advapi32",
		"runtimeobject",
		{ public = true }
	)
target_end()
