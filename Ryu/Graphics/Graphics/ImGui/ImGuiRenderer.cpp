#include "Graphics/ImGui/ImGuiRenderer.h"

#if defined(RYU_WITH_EDITOR)
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>

namespace Ryu::Gfx
{
	ImGuiDescriptorAllocator::~ImGuiDescriptorAllocator()
	{
		m_heap = nullptr;
		m_device = nullptr;
	}
	void ImGuiDescriptorAllocator::Initialize(DescriptorHeap* heap, Device* device)
	{
		m_heap = heap;
		m_device = device;
	}

	void ImGuiDescriptorAllocator::Shutdown()
	{
		// Free all allocated handles
		for (auto& handle : m_allocatedHandles)
		{
			if (handle.IsValid() && m_heap)
			{
				m_heap->Free(handle);
			}
		}
		m_allocatedHandles.clear();
	}

	void ImGuiDescriptorAllocator::Alloc(ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu)
	{
		if (auto* allocator = static_cast<ImGuiDescriptorAllocator*>(info->UserData))
		{
			DescriptorHandle handle = allocator->m_heap->Allocate();
			*out_cpu = handle.CPU;
			*out_gpu = handle.GPU;

			allocator->m_allocatedHandles.push_back(handle);
		}
	}

	void ImGuiDescriptorAllocator::Free(ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE cpu, D3D12_GPU_DESCRIPTOR_HANDLE gpu)
	{
		if (auto* allocator = static_cast<ImGuiDescriptorAllocator*>(info->UserData))
		{
			// Find and free the matching handle
			auto it = std::find_if(allocator->m_allocatedHandles.begin(), allocator->m_allocatedHandles.end(),
				[cpu, gpu](const DescriptorHandle& handle)
				{
					return handle.CPU.ptr == cpu.ptr && handle.GPU.ptr == gpu.ptr;
				});

			if (it != allocator->m_allocatedHandles.end())
			{
				if (it->IsValid() && allocator->m_heap)
				{
					allocator->m_heap->Free(*it);
				}
				allocator->m_allocatedHandles.erase(it);
			}
		}
	}

	ImGuiRenderer::ImGuiRenderer()
		: m_descriptorAllocator()
	{
	}

	void ImGuiRenderer::Initialize(Device* device, HWND window, DescriptorHeap& srvHeap, u32 frameCount, Format rtvFormat)
	{
		// Setup ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui::StyleColorsDark();

		ImGui_ImplWin32_Init(window);

		m_descriptorAllocator.Initialize(&srvHeap, device);

		ImGui_ImplDX12_InitInfo init_info{};
		init_info.Device                  = device->GetDevice();
		init_info.CommandQueue            = device->GetCommandContext().GetCommandQueue().Get();
		init_info.NumFramesInFlight       = frameCount;
		init_info.RTVFormat               = DXGI::ToNative(rtvFormat);
		init_info.DSVFormat               = DXGI_FORMAT_UNKNOWN;

		// Set up descriptor heap and allocation callbacks
		init_info.SrvDescriptorHeap    = srvHeap.Get();
		init_info.SrvDescriptorAllocFn = ImGuiDescriptorAllocator::Alloc;
		init_info.SrvDescriptorFreeFn  = ImGuiDescriptorAllocator::Free;
		init_info.UserData             = &m_descriptorAllocator;

		ImGui_ImplDX12_Init(&init_info);

		m_initialized = true;
	}

	void ImGuiRenderer::Shutdown()
	{
		if (m_initialized)
		{
			ImGui_ImplDX12_Shutdown();
			ImGui_ImplWin32_Shutdown();

			m_descriptorAllocator.Shutdown();

			ImGui::DestroyContext();
			m_initialized = false;
		}
	}

	void ImGuiRenderer::BeginFrame()
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiRenderer::EndFrame(CommandList& cmdList)
	{
		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList.Get());
	}

	void ImGuiRenderer::Render(CommandList& cmdList)
	{
		EndFrame(cmdList);
	}
}
#endif
