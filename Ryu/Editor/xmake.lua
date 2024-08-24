target("RyuEditor")
    add_rules("IncludeConfigs", "CommonPackages")

    set_default(false)
    set_kind("binary")
    set_group("Ryu")

    add_includedirs("..")
    add_files("**.cpp")
    add_headerfiles("**.h")
target_end()