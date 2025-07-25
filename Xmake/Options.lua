option("ryu-log-trace-enabled")
	set_showmenu(true)
	set_default(true)
	set_category("root Ryu/Logging")
	set_description("Compile time switch to enable trace verbosity logs")
option_end()

option("ryu-log-debug-enabled")
	set_showmenu(true)
	set_default(true)
	set_category("root Ryu/Logging")
	set_description("Compile time switch to enable debug verbosity logs")
option_end()

option("ryu-log-info-enabled")
	set_showmenu(true)
	set_default(true)
	set_category("root Ryu/Logging")
	set_description("Compile time switch to enable info verbosity logs")
option_end()

option("ryu-log-warn-enabled")
	set_showmenu(true)
	set_default(true)
	set_category("root Ryu/Logging")
	set_description("Compile time switch to enable warn verbosity logs")
option_end()

option("ryu-throw-on-fail-hresult")
	set_showmenu(true)
	set_default(true)
	set_description("Assert on failure of HRESULT")
	set_category("root Ryu/Graphics")
option_end()

option("ryu-enable-tracy-profiling")
	set_showmenu(true)
	set_default(false)
	set_description("Enable profiling")
	set_category("root Ryu/Profiling")
	add_defines("RYU_PROFILING_ENABLED", "TRACY_ENABLE")
option_end()

option("ryu-build-with-editor")
    set_default(true)
    set_showmenu(true)
    set_category("root Ryu/Game")
    set_description("Enable the game editor")
option_end()

option("ryu-unity-build")
    set_default(true)
    set_showmenu(true)
    set_category("root Ryu/Build")
    set_description("Compile using unity build")
option_end()
