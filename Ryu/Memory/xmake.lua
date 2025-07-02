target("RyuMemory")
	set_kind("static")
	set_group("Ryu/Core")

	add_deps("RyuCommon")

	add_includedirs(".", { public = true })
	add_files("./Memory/*.cpp", { unity_group = "Memory" })
	add_headerfiles("**.h")

	-- Tests
	for _, testfile in ipairs(os.files("./Tests/*.cpp")) do
		 add_tests(path.basename(testfile),
		 {
			 kind           = "binary",
			 group          = "memory",
			 files          = testfile,
			 languages      = "cxx23",
			 packages       = "doctest",
		 })
	end

	if has_config("ryu-unity-build") then
		add_rules("c++.unity_build")
	end
target_end()
