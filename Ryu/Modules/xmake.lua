target("RyuModuleTest")
	set_group("Ryu/Modules")
	set_kind("binary")

	set_policy("build.c++.modules", true)
	add_options("ryu-log-level", "ryu-enable-tracy-profiling", { public = true })

	add_packages(
		"spdlog", "uuid_v4", "Elos",
	 	"toml++", "cli11", { public = true })

	add_rules("EnumToHeader", {
		root = path.join(os.projectdir(), "Ryu", "Enums"),
		files = { "ServiceErrorType.json" },
		force = false
	})

	add_includedirs("Core", { public = true })
	add_files("Core/Common/**.ixx" , { public = true })
	add_files("Core/Globals/**.ixx", "Core/Globals/**.cpp", { public = true })
	add_files("Core/Utils/**.ixx", "Core/Utils/**.cpp", { public = true })
	add_headerfiles("Core/**.h")
target_end()
