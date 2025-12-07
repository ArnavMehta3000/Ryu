#pragma once
#include "Utils/Timing/TimerBase.h"

namespace Ryu::Utils
{
	class TimerNew final : public TimerBase
	{
	public:
		constexpr TimerNew() = default;
		
		template<DurationType T>
		explicit TimerNew(T duration, bool startImmediately = false) noexcept
			: m_targetDuration(std::chrono::duration_cast<Duration>(duration))
		{
			if (startImmediately) Start();
		}

		void Start()
		{
			if (!m_isRunning)
			{
				m_startTime = Clock::now();
				m_isRunning = true;
				m_isCompleted = false;
			}
		}

		void Stop()
		{
			if (m_isRunning)
			{
				m_accumulated += Clock::now() - m_startTime;
				m_isRunning = false;
			}
		}

		void Reset()
		{
			m_startTime   = Clock::now();
			m_accumulated = Duration::zero();
			m_isRunning   = false;
			m_isCompleted = false;
		}

		template <DurationType T>
		void SetDuration(T duration)
		{
			m_targetDuration = std::chrono::duration_cast<Duration>(duration);
		}

		[[nodiscard]] bool IsCompleted() const noexcept
		{
			if (m_isCompleted) 
			{
				return true;
			}

			auto elapsed = m_isRunning ? (Clock::now() - m_startTime + m_accumulated) : m_accumulated;
			return elapsed >= m_targetDuration;
		}

		template<DurationType T = std::chrono::milliseconds>
		[[nodiscard]] f64 Remaining() const noexcept
		{
			auto elapsed = m_isRunning ? (Clock::now() - m_startTime + m_accumulated) : m_accumulated;
			auto remaining = m_targetDuration > elapsed ? (m_targetDuration - elapsed) : Duration::zero();

			return std::chrono::duration<double, typename T::period>(remaining).count();
		}

		[[nodiscard]] f32 Progress() const noexcept
		{
			if (m_targetDuration.count() == 0)
			{
				return 1.0f;
			}

			auto elapsed = m_isRunning ? (Clock::now() - m_startTime + m_accumulated) : m_accumulated;
			return std::min(1.0f,
				static_cast<f32>(elapsed.count()) /
				static_cast<f32>(m_targetDuration.count()));
		}

	private:
		Duration m_targetDuration{};
		bool     m_isCompleted{ false };
	};
}
