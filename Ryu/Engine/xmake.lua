target("RyuEngine")
    add_rules("IncludeConfigs", "CommonPackages", "BuildAsDLL")

    set_default(false)
    set_kind("shared")
    set_group("Ryu")

    add_includedirs("..", { public = true })
    add_files("**.cpp")
    add_headerfiles("**.h")

    add_deps("RyuCore")
    add_links("RyuCore")
target_end()