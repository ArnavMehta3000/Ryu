#pragma once

namespace Ryu::Logging { class Logger; }
namespace Ryu::Config  { class ConfigManager; class CmdLine; }
namespace Ryu::App { class PathManager; class IApplication; }
namespace Ryu::Gfx     { class Renderer; class ShaderCompiler; }
namespace Ryu::Game    { class InputManager; }
namespace Ryu::Memory  { class Allocator; }

namespace Ryu::Engine
{
    // Version for ABI compatibility checking
    RYU_TODO("Use xmakes config system to automate this")
    struct EngineVersion
    {
        u32 Major;
        u32 Minor;
        u32 Patch;

        constexpr u64 AsNumber() const noexcept
        {
            return (u64(Major) << 32) | (u64(Minor) << 16) | Patch;
        }
    };

    inline constexpr EngineVersion CURRENT_VERSION = { 1, 0, 0 };
}
