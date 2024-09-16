-- Add libfmt package
add_requires("spdlog")

-- Add backward-cpp package
add_requires("backward-cpp", { debug = is_mode("debug") })

-- Add nlohmann_json package
add_requires("nlohmann_json")
