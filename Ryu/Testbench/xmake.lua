target("RyuTestbench")
    set_default(true)
    set_kind("binary")
    set_group("Ryu")

    add_includedirs("..")
    add_files("**.cpp")
    add_headerfiles("**.h")
target_end()