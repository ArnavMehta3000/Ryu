target("SimpleMath")
	set_group("Ryu/External")
	set_kind("object")
	add_headerfiles("External/SimpleMath/SimpleMath.h")
	add_files("External/SimpleMath/SimpleMath.cpp")
	add_includedirs(".", { public = true })
target_end()

target("GameInput")
	set_group("Ryu/External")
	set_kind("headeronly")
	add_extrafiles("External/GameInput/README.md")
	add_headerfiles("External/GameInput/**.h")
	add_links(path.translate("$(scriptdir)/External/External/GameInput/GameInput.lib"), { public = true })
target_end()

target("ImGui")
	set_group("Ryu/External")
	set_kind("object")
	add_headerfiles(
		"External/ImGui/*.h",
		"External/ImGui/backends/*.h")

	add_files(
		"External/ImGui/*.cpp",
		"External/ImGui/backends/*.cpp")

	add_includedirs(".", { public = true })
	add_includedirs("External/ImGui", { public = true })
target_end()

target("STB")
	set_group("Ryu/External")
	set_kind("object")
	add_headerfiles("External/STB/*.h")
	add_files("External/STB/*.cpp")
	add_includedirs(".", { public = true })
target_end()

target("TinyOBJ")
	set_group("Ryu/External")
	set_kind("headeronly")
	add_headerfiles("External/TinyOBJ/*.h")
	add_includedirs(".", { public = true })
target_end()
