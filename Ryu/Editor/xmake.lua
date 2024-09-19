-- Editor uses imgui
add_requires("imgui docking", { configs= { dx11 = true, win32 = true }})

target("RyuEditor")
    add_rules("IncludeConfigs", "CommonPackages")
    add_rules("win.sdk.application")
    add_packages("imgui")

    set_default(true)
    set_kind("binary")
    set_group("Ryu")

    add_includedirs("..")
    add_files("**.cpp")
    add_headerfiles("**.h")

    add_deps("RyuCore", "RyuEngine")
    add_links("RyuCore", "RyuEngine")
target_end()
