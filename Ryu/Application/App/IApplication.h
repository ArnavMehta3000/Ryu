#pragma once

namespace Ryu::Utils  { class FrameTimer; }
namespace Ryu::Window { class Window; }
namespace Ryu::Game   { class World;  class WorldManager; }
namespace Ryu::Gfx    { class IRendererHook; }
namespace Ryu::Engine { class Engine; struct EngineConfig; }
namespace Ryu::Editor { class EditorApp; }

namespace Ryu::App
{
    class IApplication
    {
    public:
        virtual ~IApplication() = default;

        [[nodiscard]] virtual bool OnInit() = 0;
        virtual void OnTick(const Utils::FrameTimer& timer) = 0;
        virtual void OnShutdown() = 0;

        [[nodiscard]] virtual Window::Window* GetWindow() = 0;
        [[nodiscard]] virtual Game::WorldManager* GetWorldManager() { return nullptr; }
        [[nodiscard]] virtual Gfx::IRendererHook* GetRendererHook() { return nullptr; }

        [[nodiscard]] virtual bool IsRunning() const = 0;
        virtual void RequestQuit() = 0;
    };
}
