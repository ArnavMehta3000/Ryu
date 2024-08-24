target("RyuEngine")
    add_rules("IncludeConfigs")

    set_default(false)
    set_kind("shared")
    set_group("Ryu")

    add_includedirs("..", { public = true })
    add_files("**.cpp")
    add_headerfiles("**.h")

    add_defines("RYU_BUILD_DLL", "RYU_EXPORT")
    add_packages("fmt")
target_end()