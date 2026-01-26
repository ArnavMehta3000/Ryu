
option("nvrhi-shared",     { default = false, description = "Build NVRHI as a shared library"                           })
option("nvrhi-validation", { default = true,  description = "Build NVRHI with validation layer"                         })
option("nvrhi-vulkan",     { default = false,  description = "Build the NVRHI Vulkan backend"                            })
option("nvrhi-rtxmu",      { default = false, description = "Use RTXMU for acceleration structure management"           })
option("nvrhi-aftermath",  { default = false, description = "Include Aftermath support (requires NSight Aftermath SDK)" })

if is_plat("windows") then
	option("nvrhi-nvapi",                  { default = false, description = "Include NVAPI support (requires NVAPI SDK)"      })
	option("nvrhi-dx11",                   { default = false,  description = "Build the NVRHI D3D11 backend"                   })
	option("nvrhi-dx12",                   { default = true,  description = "Build the NVRHI D3D12 backend"                   })
	option("nvrhi-dx12_opacity_micromap",  { default = true,  description = "Use D3D12 native Opacity Micromaps from DXR 1.2" })
end


target("NVRHI")
	set_group("Ryu/External/NVRHI")

	if has_config("nvrhi-shared") then
		set_kind("shared")
		add_defines("NVRHI_SHARED_LIBRARY_BUILD=1")
	else
		set_kind("static")
	end

	add_includedirs("NVRHI/src", {private = true})
	add_includedirs("NVRHI/include", { public = true })
	add_extrafiles("NVRHI/tools/nvrhi.natvis")

	add_files("NVRHI/src/common/**.cpp")
	add_headerfiles(
		"NVRHI/include/nvrhi/common/**.h",
		"NVRHI/include/nvrhi/nvrhiHLSL.h",
		"NVRHI/include/nvrhi/utils.h",
		"NVRHI/include/nvrhi/nvrhi.h",
		"NVRHI/src/common/**.h"
	)

	if has_config("nvrhi-validation") then
		add_files("NVRHI/src/validation/**.cpp")
		add_headerfiles("NVRHI/include/nvrhi/validation.h", "NVRHI/src/validation/**.h")
	end

	if has_config("nvrhi-aftermath") then
		add_defines("NVRHI_WITH_AFTERMATH=1")
		-- Note: Need to add the Aftermath SDK path manually
		-- add_includedirs("path/to/aftermath/include")
		-- add_linkdirs("path/to/aftermath/lib")
		add_links("aftermath")
	else
		add_defines("NVRHI_WITH_AFTERMATH=0")
	end

	if has_config("nvrhi-shared") and has_config("nvrhi-vulkan") then
		add_defines("VULKAN_HPP_STORAGE_SHARED", "VULKAN_HPP_STORAGE_SHARED_EXPORT")
	end
target_end()

-- D3D11 backend
if is_plat("windows") and has_config("nvrhi-dx11") then
	if has_config("nvrhi-shared") then

		-- Add D3D11 sources to main nvrhi target
		target("NVRHI")

			add_files("NVRHI/src/d3d11/**.cpp")
			add_headerfiles("NVRHI/include/nvrhi/d3d11.h", "NVRHI/src/d3d11/**.h")

			add_defines("NVRHI_HAS_D3D11", { public = true })

			add_syslinks("d3d11", "dxguid", "dxgi")

			if has_config("nvrhi-nvapi") then
				add_defines("NVRHI_D3D11_WITH_NVAPI=1")
				add_links("nvapi")
			else
				add_defines("NVRHI_D3D11_WITH_NVAPI=0")
			end
		target_end()
	else
		-- Create separate D3D11 static library
		target("NVRHI-D3D11")
			set_group("Ryu/External/NVRHI")
			set_kind("static")

			add_files("NVRHI/src/d3d11/**.cpp")
			add_headerfiles("NVRHI/include/nvrhi/d3d11.h", "NVRHI/src/d3d11/**.h")

			add_defines("NVRHI_HAS_D3D11", { public = true })

			add_includedirs("NVRHI/include")

			add_syslinks("d3d11", "dxguid", "dxgi")

			if has_config("nvrhi-nvapi") then
				add_defines("NVRHI_D3D11_WITH_NVAPI=1")
				add_links("nvapi")
			else
				add_defines("NVRHI_D3D11_WITH_NVAPI=0")
			end

			if has_config("nvrhi-aftermath") then
				add_defines("NVRHI_WITH_AFTERMATH=1")
			else
				add_defines("NVRHI_WITH_AFTERMATH=0")
			end
		target_end()
	end
end

-- D3D12 Backend
if is_plat("windows") and has_config("nvrhi-dx12") then
	if has_config("shared") then
		target("NVRHI")
			add_files("NVRHI/src/d3d12/**.cpp")
			add_headerfiles("NVRHI/include/nvrhi/d3d12.h", "NVRHI/src/d3d12/**.h")

			add_defines("NVRHI_HAS_D3D12", { public = true })

			add_syslinks("d3d12", "dxgi", "dxguid")

			-- DirectX-Headers
			add_includedirs("NVRHI/thirdparty/DirectX-Headers/include", { public = true })

			if has_config("nvrhi-rtxmu") then
				add_defines("NVRHI_WITH_RTXMU=1")
				add_deps("rtxmu")
			end

			if has_config("nvrhi-dx12_opacity_micromap") then
				add_defines("NVRHI_D3D12_WITH_DXR12_OPACITY_MICROMAP=1")
			else
				add_defines("NVRHI_D3D12_WITH_DXR12_OPACITY_MICROMAP=0")
			end

			if has_config("nvrhi-nvapi") then
				add_defines("NVRHI_D3D12_WITH_NVAPI=1")
				add_links("nvapi")
			else
				add_defines("NVRHI_D3D12_WITH_NVAPI=0")
			end
		target_end()
	else
		-- Create separate D3D12 static library
		target("NVRHI-D3D12")
			set_group("Ryu/External/NVRHI")
			set_kind("static")

			add_files("NVRHI/src/d3d12/**.cpp")
			add_headerfiles("NVRHI/include/nvrhi/d3d12.h", "NVRHI/src/d3d12/**.h")

			add_defines("NVRHI_HAS_D3D12", { public = true })

			add_includedirs("NVRHI/include")
			add_syslinks("d3d12", "dxgi", "dxguid")

			-- DirectX-Headers
			add_includedirs("NVRHI/thirdparty/DirectX-Headers/include", { public = true })

			if has_config("nvrhi-rtxmu") then
				add_defines("NVRHI_WITH_RTXMU=1")
				add_deps("rtxmu")
			end

			if has_config("nvrhi-dx12_opacity_micromap") then
				add_defines("NVRHI_D3D12_WITH_DXR12_OPACITY_MICROMAP=1")
			else
				add_defines("NVRHI_D3D12_WITH_DXR12_OPACITY_MICROMAP=0")
			end

			if has_config("nvrhi-nvapi") then
				add_defines("NVRHI_D3D12_WITH_NVAPI=1")
				add_links("nvapi")
			else
				add_defines("NVRHI_D3D12_WITH_NVAPI=0")
			end

			if has_config("nvrhi-aftermath") then
				add_defines("NVRHI_WITH_AFTERMATH=1")
			else
				add_defines("NVRHI_WITH_AFTERMATH=0")
			end
		target_end()
	end
end

if has_config("nvrhi-vulkan") then
	if has_config("nvrhi-shared") then
        -- Add Vulkan sources to main nvrhi target
        target("NVRHI")
			add_files("NVRHI/src/vulkan/**.cpp")
			add_headerfiles("NVRHI/include/nvrhi/vulkan.h", "NVRHI/src/vulkan/**.h")

			add_defines("NVRHI_HAS_VULKAN", { public = true })

			add_syslinks("vulkan")
			add_includedirs("NVRHI/thirdparty/Vulkan-Headers/include", { public = true })

            if is_plat("windows") then
                add_defines("VK_USE_PLATFORM_WIN32_KHR", "NOMINMAX")
            end

            if has_config("nvrhi-rtxmu") then
                add_defines("NVRHI_WITH_RTXMU=1")
                add_deps("rtxmu")
            end
        target_end()
    else
		target("NVRHI-Vulkan")
			set_group("Ryu/External/NVRHI")
            set_kind("static")

			add_files("NVRHI/src/vulkan/**.cpp")
			add_headerfiles("NVRHI/include/nvrhi/vulkan.h", "NVRHI/src/vulkan/**.h")

			add_defines("NVRHI_HAS_VULKAN", { public = true })

			add_syslinks("vulkan")
			add_includedirs("NVRHI/thirdparty/Vulkan-Headers/include", { public = true })

            add_includedirs("NVRHI/include")

            if is_plat("windows") then
                add_defines("VK_USE_PLATFORM_WIN32_KHR", "NOMINMAX")
            end

            if has_config("nvrhi-rtxmu") then
                add_defines("NVRHI_WITH_RTXMU=1")
                add_deps("rtxmu")
            end

            if has_config("nvrhi-aftermath") then
                add_defines("NVRHI_WITH_AFTERMATH=1")
            else
                add_defines("NVRHI_WITH_AFTERMATH=0")
            end
        target_end()
	end
end
