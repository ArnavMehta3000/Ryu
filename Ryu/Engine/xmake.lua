target("RyuEngine")
    set_default(false)
    set_kind("shared")
    set_group("Ryu")

    add_includedirs("..")
    add_files("**.cpp")
    add_headerfiles("**.h")
target_end()