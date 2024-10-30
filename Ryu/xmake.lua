-- Phony target	for the core library
target("RyuCore")
	set_default(true)
	set_group("Ryu/Core")
	set_kind("phony")
	
	add_deps(
		"RyuCommon",
		"RyuUtils",
		"RyuLogger",
		"RyuEvents",
		"RyuConfig",
		"RyuInput",
		"RyuApp",
		"RyuPlugin",
		"RyuMath",
		{ public = true }
	)

	add_links(
		"RyuUtils",
		"RyuLogger",
		"RyuInput",
		"RyuPlugin",
		"RyuApp",
		{ public = true }
	)
target_end()
