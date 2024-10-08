add_requires("nlohmann_json")

add_requires("fmt", { configs = { unicode = true }})

add_requires("directx-headers")

add_requires("imgui docking", { configs = { dx11 = true, dx12 = true, win32 = true }})

add_requires("libassert")
