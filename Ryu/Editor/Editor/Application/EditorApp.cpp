#include "EditorApp.h"
#include "Game/IGameModule.h"
#include "Logging/Logger.h"
#include "Profiling/Profiling.h"
#include "Graphics/Core/GfxDevice.h"
#include <External/ImGui/backends/imgui_impl_win32.h>
#include <External/ImGui/backends/imgui_impl_dx12.h>
#include <External/ImGui/imgui.h>

namespace Ryu::Editor
{
	namespace
	{
		// Handle to the original application window procedure
		WNDPROC g_originalWndProc{ nullptr };

		static void ImGui_SrvDescriptorAlloc(
			ImGui_ImplDX12_InitInfo* info,
			D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_desc_handle,
			D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_desc_handle)
		{
			if (auto* heap = static_cast<Gfx::DescriptorHeap*>(info->UserData))
			{
				Gfx::DescriptorHandle handle = heap->Allocate();

				if (handle.IsValid())
				{
					*out_cpu_desc_handle = handle.CPU;
					*out_gpu_desc_handle = handle.GPU;
				}
				else
				{
					RYU_LOG_ERROR("Failed to allocate descriptor for ImGui!");
				}
			}
			else
			{
				RYU_LOG_ERROR("Invalid user data while trying to allocate descriptor for ImGui!");
			}
		}

		static void ImGui_SrvDescriptorFree(
			ImGui_ImplDX12_InitInfo* info,
			D3D12_CPU_DESCRIPTOR_HANDLE cpu_desc_handle,
			[[maybe_unused]]  D3D12_GPU_DESCRIPTOR_HANDLE gpu_desc_handle)
		{
			if (auto* heap = static_cast<Gfx::DescriptorHeap*>(info->UserData))
			{
				// Find the index from the cpu handle, convert to full handle and free it
				u32 index = heap->FindIndex(cpu_desc_handle);
				const Gfx::DescriptorHandle handle = heap->GetHandle(index);
				heap->Free(handle);
			}
			else
			{
				RYU_LOG_ERROR("Invalid user data while trying to free descriptor for ImGui!");
			}
		}
	}

	EditorApp::EditorApp(std::shared_ptr<Window::Window> window)
		: App::App(window)
	{
	}

	bool EditorApp::OnInit()
	{
		RYU_PROFILE_SCOPE();

		RYU_LOG_TRACE("Initializing editor application");

		if (!LoadGameModule())
		{
			RYU_LOG_ERROR("Failed to load game module!");
			return false;
		}

		//Gfx::Renderer* renderer = Engine::Engine::Get().GetRenderer();
		//renderer->ImGuiCallback = [this](Gfx::Renderer* renderer) { OnImGui(renderer); };

		// Init user application
		RYU_LOG_TRACE("Initializing user application");
		m_userApp->m_isRunning = m_userApp->OnInit();
		return m_userApp->m_isRunning;
	}

	void EditorApp::OnShutdown()
	{
		RYU_PROFILE_SCOPE();

		RYU_LOG_TRACE("Shutting down editor application");

		m_userApp->OnShutdown();

		m_userApp.reset();

		RYU_LOG_INFO("Editor application shutdown");
	}

	void EditorApp::OnTick(const Utils::TimeInfo& timeInfo)
	{
		RYU_PROFILE_SCOPE();

		// Display FPS stats in debug mode
#if defined(RYU_BUILD_DEBUG)
		GetWindow()->SetTitle(fmt::format("Ryu Editor | DT: {:.5f}ms | FPS: {}", timeInfo.DeltaTime, timeInfo.FPS));
#endif

		m_userApp->OnTick(timeInfo);
	}

	bool EditorApp::LoadGameModule()
	{
		RYU_PROFILE_SCOPE();

		std::unique_ptr<Game::IGameModule> gm(Game::CreateGameModule());
		if (gm)
		{
			m_userApp = gm->CreateApplication(GetWindow());
			GetWindow()->SetTitle(fmt::format("Ryu Editor - {}", gm->GetName()));
			return m_userApp != nullptr;
		}

		return false;
	}

	void EditorApp::OnImGuiSetup(Gfx::Device* device)
	{
		m_imguiHeap = std::make_unique<Gfx::DescriptorHeap>(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 64, true, "ImGui SRV Heap");

		const HWND hWnd = GetWindow()->GetHandle();
		ImGui_ImplWin32_EnableDpiAwareness();
		const f32 dpiScale = ImGui_ImplWin32_GetDpiScaleForHwnd(hWnd);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		style.ScaleAllSizes(dpiScale);
		style.FontScaleDpi = dpiScale;

		ImGui_ImplDX12_InitInfo info{};
		info.Device               = device->GetNativeDevice();
		info.CommandQueue         = device->GetCommandQueue()->GetNative();
		info.NumFramesInFlight    = Gfx::FRAME_BUFFER_COUNT;
		info.RTVFormat            = DXGI_FORMAT_R8G8B8A8_UNORM;
		info.DSVFormat            = DXGI_FORMAT_UNKNOWN;
		info.SrvDescriptorHeap    = m_imguiHeap->GetNative();
		info.SrvDescriptorAllocFn = ImGui_SrvDescriptorAlloc;
		info.SrvDescriptorFreeFn  = ImGui_SrvDescriptorFree;
		info.UserData             = m_imguiHeap.get();

		bool success = ImGui_ImplWin32_Init(hWnd);
		success = ImGui_ImplDX12_Init(&info);
	}

	void EditorApp::OnImGuiFrameBegin()
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void EditorApp::OnImGuiFrameEnd(Gfx::CommandList* cmdList)
	{
		cmdList->SetDescriptorHeap(*m_imguiHeap);

		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList->GetNative());

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}

	void EditorApp::OnImGuiRender()
	{
		ImGui::ShowDemoWindow();
	}

	void EditorApp::OnImGuiShutdown()
	{
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

}
