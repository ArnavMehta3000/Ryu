add_requires("fmt", { configs = { unicode = true }})

add_requires("directx-headers")

add_requires("imgui docking", { configs = { dx11 = true, dx12 = true, win32 = true }})

add_requires("libassert")

add_requires("uuid_v4")

add_requires("entt")

add_requires("Elos 98d44a142953be2eaab83030d3d1f527ebf81978")

add_requires("toml++")

package("TracyServerPackage")
	set_sourcedir(path.join(os.projectdir(), "External", "External", "Tracy", "profiler"))
	on_install(function (package)
		local configs = {}
		table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. "Release")

		import("package.tools.cmake").install(package, configs)

		-- Copy the generated exe
		local exe_dir = path.join(package:buildir(), "Release")
		local copy_path = path.join(os.projectdir(), "build", "Programs", "TracyServer")
		os.cp(exe_dir, copy_path)
		cprint(format("${cyan}Tracy server installed at: %s${clear}", copy_path))
	end)
package_end()
add_requires("TracyServerPackage")