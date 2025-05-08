target("RyuGraphics")
	set_kind("static")
	set_group("Ryu/Graphics")

	if has_config("ryu-assert-on-fail-hresult") then
		add_defines("RYU_BREAK_ON_FAIL_HRESULT")
	end

	add_includedirs(".", { public = true })
	add_files("**.cpp", { unity_group = "Graphics" })
	add_headerfiles("**.h")

	add_deps("RyuGraphicsRHI")
	add_rules("c++.unity_build")

	after_load(function (target)
		import("core.project.project")

		local dx11_target = project.target("RyuGraphicsDX11")
		local dx12_target = project.target("RyuGraphicsDX12")

		-- If both targets are nil, then fail compile at build time
		if dx11_target == nil and dx12_target == nil then
			cprint("${red}Both DX11 and DX12 graphics modules are disabled!${clear}")
			raise("At least one graphics API should be enabled!")
		end

		-- Force add DX11 as a dependency, regardless of its enabled state
		if dx11_target and dx11_target:is_enabled() then
			target:add("deps", "RyuGraphicsDX11", { public = true })
			target:add("defines", "RYU_RHI_D3D11", { public = true })
		end
		-- Check if DX12 target is enabled and add as dependency
		if dx12_target and dx12_target:is_enabled() then
			target:add("deps", "RyuGraphicsDX12", { public = true })
			target:add("defines", "RYU_RHI_D3D12", { public = true })
		end
	end)
target_end()
