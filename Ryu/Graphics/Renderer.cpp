#include "Graphics/Renderer.h"

#include "Core/Profiling/Profiling.h"
#include "Core/Utils/Timing/FrameTimer.h"
#include "Game/World/World.h"
#include "Graphics/CommonStates.h"
#include "Graphics/Compiler/ShaderCompiler.h"
#include "Graphics/Core/GfxTexture.h"
#include "Graphics/IRendererHook.h"
#include "Graphics/RenderFrameBuilder.h"

namespace Ryu::Gfx
{
    Renderer::Renderer(HWND window, IRendererHook* hook)
        : m_device(std::make_unique<Device>(window))
        , m_gpuFactory(m_device.get())
        , m_assets(&m_gpuFactory)
        , m_shaderLibrary("./Shaders/Compiled"sv)
        , m_worldRenderer(m_device.get(), &m_shaderLibrary, &m_assets)
        , m_hook(hook)
    {
        RYU_PROFILE_SCOPE();

        m_device->Initialize();

        const auto [w, h] = m_device->GetClientSize();

#if defined(RYU_WITH_EDITOR)
        if (m_hook)
        {
            m_hook->OnImGuiSetup(m_device.get());
        }
#endif
    }

    Renderer::~Renderer()
    {
#if defined(RYU_WITH_EDITOR)
        if (m_hook)
        {
            m_hook->OnImGuiShutdown();
        }
#endif
    }

    void Renderer::RenderWorld(Game::World& world, const Utils::FrameTimer& frameTimer)
    {
        RYU_PROFILE_SCOPE();

        RenderFrameBuilder builder(&m_assets, m_device.get());

        const Gfx::RenderFrame frameData = builder.ExtractRenderData(world, frameTimer);
        m_worldRenderer.RenderFrame(frameData, &m_gpuFactory, m_hook);
    }

    void Renderer::OnResize(u32 w, u32 h)
    {
        RYU_PROFILE_SCOPE();

        m_worldRenderer.OnResize(w, h);  // This will call resize on the device;
    }
}
