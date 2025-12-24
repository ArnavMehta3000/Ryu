#pragma once
#include "Core/Utils/Timing/TimerBase.h"

namespace Ryu::Utils
{
	class Stopwatch final : public TimerBase
	{
	public:
		constexpr Stopwatch(bool startImmediately = false)
		{
			if (startImmediately)
			{
				Start();
			}
		}

		void Start() noexcept
		{
			if (!m_isRunning)
			{
				m_startTime = Clock::now();
				m_isRunning = true;
			}
		}

		void Stop() noexcept
		{
			if (m_isRunning)
			{
				m_accumulated += Clock::now() - m_startTime;
				m_isRunning = false;
			}
		}

		void Reset() noexcept
		{
			m_startTime   = Clock::now();
			m_accumulated = Duration::zero();
			m_isRunning   = false;
		}

		void Restart() noexcept
		{
			m_accumulated = Duration::zero();
			m_startTime   = Clock::now();
			m_isRunning   = true;
		}

		template<DurationType T = std::chrono::milliseconds>
		[[nodiscard]] f64 Lap() noexcept
		{
			auto now = Clock::now();
			auto lapTime = m_isRunning ? (now - m_startTime) : Duration::zero();

			if (m_isRunning)
			{
				m_accumulated += lapTime;
				m_startTime = now;
			}

			return std::chrono::duration<f64, typename T::period>(lapTime).count();
		}
	};
}
