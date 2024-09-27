-- Add libfmt package
add_requires("spdlog")

-- Add backward-cpp package
add_requires("backward-cpp", { debug = is_mode("debug") })

-- Add nlohmann_json package
add_requires("nlohmann_json")

-- Add dx12 headers
add_requires("directx-headers")

-- Add ImGui for Win32 with DX11 and DX12 backends
add_requires("imgui docking", { configs= { dx11 = true, dx12 = true, win32 = true }})
