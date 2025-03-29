target("SimpleMath")
	set_group("Ryu/External")
	set_kind("static")
	add_headerfiles("External/SimpleMath/SimpleMath.h")
	add_files("External/SimpleMath/SimpleMath.cpp")
target_end()

target("StepTimer")
	set_group("Ryu/External")
	set_kind("headeronly")
	add_headerfiles("External/StepTimer/StepTimer.h")
target_end()

target("TracyClient")
	set_group("Ryu/External")
	set_kind("static")
	add_options("ryu-enable-tracy-profiling", { public = true })
	add_includedirs("External/Tracy/public", { public = true })
	add_headerfiles("External/Tracy/public/**.h")
	add_files("External/Tracy/public/TracyClient.cpp")
target_end()


package("TracyServer")
	set_sourcedir(path.join(os.scriptdir(), "External", "Tracy", "profiler"))
	on_install(function (package)
		
		local configs = {}
		table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. "Release")
		
		import("package.tools.cmake").install(package, configs)

		-- Copy the generated exe
		local exe_dir = path.join(package:buildir(), "Release")
		local copy_path = path.join(os.projectdir(), "build", "Programs", "TracyServer")
		os.cp(exe_dir, copy_path)
	end)
package_end()



-- A phony target to include all external source files
target("RyuExternals")
	set_group("Ryu/External")
	set_kind("phony")
	add_includedirs(".", { public = true })

	add_deps(
		"SimpleMath",
		"StepTimer",
		{ public = true })
target_end()
