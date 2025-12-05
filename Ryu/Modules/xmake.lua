target("RyuModuleCore")
	set_group("Ryu/Modules")
	set_kind("object")

	set_policy("build.c++.modules", true)

	add_includedirs("Core", { public = true })
	add_files("Core/Common/**.ixx")
target_end()
