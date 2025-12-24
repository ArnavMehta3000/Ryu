#pragma once
#include "Core/Utils/Timing/TimerBase.h"
#include <optional>

namespace Ryu::Utils
{
    class FrameTimer final
    {
    public:
        FrameTimer() noexcept
            : m_startTime(Clock::now())
            , m_lastFrameTime(Clock::now())
            , m_deltaTime(0.0)
            , m_totalFrameCount(0)
            , m_fpsFrameCount(0)
            , m_fpsUpdateInterval(std::chrono::seconds(1))
            , m_fpsAccumulator(Duration::zero())
            , m_fps(0.0)
        {
        }

        void Tick() noexcept
        {
            auto now = Clock::now();
            auto frameDuration = now - m_lastFrameTime;

            m_deltaTime     = std::chrono::duration<f64>(frameDuration).count();
            m_lastFrameTime = now;

            // FPS calculation
            m_totalFrameCount++;
            m_fpsFrameCount++;
            m_fpsAccumulator += frameDuration;

            if (m_fpsAccumulator >= m_fpsUpdateInterval)
            {
                m_fps = static_cast<f64>(m_fpsFrameCount) /
                    std::chrono::duration<f64>(m_fpsAccumulator).count();
                m_fpsFrameCount = 0;
                m_fpsAccumulator = Duration::zero();
            }
        }

        [[nodiscard]] f64 DeltaTime() const noexcept { return m_deltaTime; }
        [[nodiscard]] f32 DeltaTimeF() const noexcept { return static_cast<f32>(m_deltaTime); }
        [[nodiscard]] f64 FPS() const noexcept { return m_fps; }
        [[nodiscard]] u64 FrameCount() const noexcept { return m_totalFrameCount; }

        template<DurationType T = std::chrono::milliseconds>
        [[nodiscard]] auto TimeSinceStart() const noexcept -> f64
        {
            auto elapsed = Clock::now() - m_startTime;
            return std::chrono::duration<f64, typename T::period>(elapsed).count();
        }

        template<DurationType T = std::chrono::milliseconds>
        [[nodiscard]] auto TimeSinceStartDuration() const noexcept -> T
        {
            auto elapsed = Clock::now() - m_startTime;
            return std::chrono::duration_cast<T>(elapsed);
        }

        void SetMaxDeltaTime(f64 maxSeconds) noexcept
        {
            m_maxDeltaTime = maxSeconds;
        }

        [[nodiscard]] f64 GetClampedDeltaTime() const noexcept
        {
            return m_maxDeltaTime.has_value() ?
                std::min(m_deltaTime, m_maxDeltaTime.value()) : m_deltaTime;
        }

        void SetFPSUpdateInterval(std::chrono::milliseconds interval) noexcept
        {
            m_fpsUpdateInterval = interval;
        }

        // Reset without losing FPS tracking
        void Reset() noexcept
        {
            m_lastFrameTime = Clock::now();
            m_deltaTime = 0.0;
        }

        // Completely reset including FPS and frame count
        void ResetAll() noexcept
        {
            m_startTime       = Clock::now();
            m_lastFrameTime   = Clock::now();
            m_deltaTime       = 0.0;
            m_totalFrameCount = 0;
            m_fpsFrameCount   = 0;
            m_fpsAccumulator  = Duration::zero();
            m_fps             = 0.0;
        }

    private:
        TimePoint          m_startTime;
        TimePoint          m_lastFrameTime;
        f64                m_deltaTime;
        u64                m_totalFrameCount;
        u64                m_fpsFrameCount;
        Duration           m_fpsUpdateInterval;
        Duration           m_fpsAccumulator;
        f64                m_fps;
        std::optional<f64> m_maxDeltaTime;
    };
}
