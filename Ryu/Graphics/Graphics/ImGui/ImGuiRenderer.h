#pragma once

#if defined(RYU_WITH_EDITOR)
#include "Graphics/Core/Device.h"
#include "Graphics/Core/DescriptorHeap.h"

#include <imgui_impl_dx12.h>

namespace Ryu::Gfx
{
    class ImGuiDescriptorAllocator
    {
    public:
        ImGuiDescriptorAllocator() = default;
        ~ImGuiDescriptorAllocator();

        void Initialize(DescriptorHeap* heap, Device* device);
        void Shutdown();

        static void Alloc(ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu);
        static void Free(ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE cpu, D3D12_GPU_DESCRIPTOR_HANDLE gpu);

    private:
        DescriptorHeap* m_heap = nullptr;
        Device* m_device = nullptr;
        std::vector<DescriptorHandle> m_allocatedHandles;
    };

    class ImGuiRenderer
    {
    public:
        ImGuiRenderer();
        ~ImGuiRenderer() = default;

        void Initialize(Device* device, HWND window, DescriptorHeap& srvHeap, u32 frameCount, Format rtvFormat);
        void Shutdown();
        void BeginFrame();
        void EndFrame(CommandList& cmdList);
        void Render(CommandList& cmdList);

    private:
        ImGuiDescriptorAllocator m_descriptorAllocator;
        bool m_initialized = false;
    };
}
#endif
