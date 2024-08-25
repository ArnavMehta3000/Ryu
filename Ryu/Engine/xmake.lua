target("RyuEngine")
    add_rules("IncludeConfigs", "CommonPackages")
    add_packages("libsdl")

    set_default(false)
    set_kind("shared")
    set_group("Ryu")

    add_includedirs("..", { public = true })
    add_files("**.cpp")
    add_headerfiles("**.h")

    add_defines("RYU_BUILD_DLL", "RYU_EXPORT")
target_end()