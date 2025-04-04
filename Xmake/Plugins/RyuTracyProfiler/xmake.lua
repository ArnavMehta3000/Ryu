task("ryu-tracy")
	set_category("plugin")
	set_menu{
		usage = "xmake ryu-tracy",
		description = "Download and run the tracy profiler (build/Programs/Tracy) ",
		options = 
		{
			{'i', "install",   "k",  nil, "Install Tracy profiler." },
			{'r', "run",       "k",  nil, "Run Tracy profiler."     },
			{nil, "ryu-tracy", "v",  nil, "The plugins path or url.",
										  "e.g.",
										 "xmake plugin --install",
										 "xmake plugin --run"}
		}
	}

	on_run("main")
task_end()
