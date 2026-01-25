includes("nvrhi-xmake.lua")

-------------------- Simple Math --------------------
target("SimpleMath")
	set_group("Ryu/External")
	set_kind("static")
	add_headerfiles("SimpleMath/SimpleMath.h", { public = true })
	add_files("SimpleMath/SimpleMath.cpp", { public = true })
	add_includedirs(".", { public = true })
target_end()

-------------------- Game Input --------------------
target("GameInput")
	set_group("Ryu/External")
	set_kind("headeronly")
	add_extrafiles("GameInput/README.md")
	add_headerfiles("GameInput/**.h", { public = true })
	add_links(path.translate("$(scriptdir)/External/GameInput/GameInput.lib"), { public = true })
target_end()

-------------------- ImGui --------------------
target("ImGui")
	set_group("Ryu/External")
	set_kind("static")
	add_headerfiles(
		"ImGui/*.h",
		"ImGui/backends/*.h", { public = true })

	add_files(
		"ImGui/*.cpp",
		"ImGui/backends/*.cpp", { public = true })

	add_includedirs(".", "ImGui", { public = true })
target_end()

-------------------- STB --------------------
target("STB")
	set_group("Ryu/External")
	set_kind("static")
	add_headerfiles("STB/*.h", { public = true })
	add_files("STB/*.cpp", { public = true })
	add_includedirs(".", { public = true })
target_end()

target("TinyOBJ")
	set_group("Ryu/External")
	set_kind("static")
	add_headerfiles("TinyOBJ/*.h", { public = true })
	add_files("TinyOBJ/*.cpp", { public = true })
	add_includedirs(".", { public = true })
target_end()
