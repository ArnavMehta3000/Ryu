target("RyuCommon")
	set_group("Ryu/Core")
	set_kind("headeronly")

	add_includedirs(".", { public = true })
	add_headerfiles("**.h")
target_end()

option("use-std-types-globally")
	set_showmenu(true)
	set_default(true)
	set_description("Bring Ryu::Common::StandardTypes to the global namespace")
	set_category("root Ryu")
option_end()
