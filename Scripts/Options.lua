-- Defines the option to choose logging level
option("loglevel")
	set_description("Choose log verbosity level",
					"- None",
					"- Warn",
					"- Info",
					"- Debug",
					"- Trace")

	set_values("None", "Warn", "Info", "Debug", "Trace")

	set_showmenu(true)
	set_category("Ryu")
option_end()

option("console")
	set_description("Create log console")
	set_showmenu(true)
	set_category("Ryu")
option_end()
