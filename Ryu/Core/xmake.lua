target("RyuCore")
    add_rules("GenConfigs", "CommonPackages")

    set_default(false)
    set_kind("static")
    set_group("Ryu")

    add_includedirs("..", { public = true })
    add_files("**.cpp")
    add_headerfiles("**.h")
target_end()
