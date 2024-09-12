target("RyuTestCoreUtils")
	set_group("Ryu/Tests")
	add_rules("IncludeConfigs", "CommonPackages")
	add_files("Main.cpp")
	set_default(false)

	for _, testfile in ipairs(os.filedirs("Utils/**.cpp")) do
		add_tests(path.basename(testfile), 
		{
			files = testfile,
			remove_files = "Main.cpp",
			group = "CoreTests"
		})
	 end

	 add_deps("RyuCore")
	 add_links("RyuCore")
target_end()
