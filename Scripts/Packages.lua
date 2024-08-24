-- Add libfmt package
add_requires("spdlog")

-- Add backward-cpp package
add_requires("backward-cpp", { debug = is_mode("debug") })
