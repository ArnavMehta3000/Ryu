add_requires("fmt", { configs = { unicode = true }})

add_requires("directx-headers")

add_requires("imgui docking", { configs = { dx11 = true, dx12 = true, win32 = true }})

add_requires("libassert")

add_requires("uuid_v4")

add_requires("entt")

add_requires("Elos")

add_requires("toml++")

add_requires("tracy 5d542dc09f3d9378d005092a4ad446bd405f819a")

add_requires("doctest")
