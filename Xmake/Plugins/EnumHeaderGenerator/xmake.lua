task("ryu-enum-to-header")
	set_category("plugin")
	set_menu{
		usage = "xmake ryu-enum-to-header",
		description = "Takes in a list of enum description JSON files and converts them to headers",
		options =
		{
			{'s', "source",   "vs",  nil, "Input source json files." }
		}
	}
	on_run("main")
task_end()
