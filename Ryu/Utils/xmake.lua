target("RyuUtils")
	set_group("Ryu/Core")
	set_kind("static")
	add_includedirs(".", { public = true })
	add_files("**.cpp", { unity_group = "Utilities" })
	add_headerfiles("**.h")
	add_extrafiles("**.inl")

	add_packages("uuid_v4", "Elos")

	add_rules("EnumToHeader",
		{
			root = path.join(os.projectdir(), "Ryu", "Enums"),
			files =
			{
				"ServiceErrorType.json"
			},
			force = false
		})

	add_deps("RyuCommon")

	if has_config("ryu-unity-build") then
		add_rules("c++.unity_build")
	end
target_end()
