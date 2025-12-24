#pragma once
#include "Core/Common/StandardTypes.h"
#include "Core/Common/Concepts.h"
#include <chrono>
#include <optional>

namespace Ryu::Utils
{
	using Clock     = std::chrono::high_resolution_clock;
	using TimePoint = Clock::time_point;
	using Duration  = Clock::duration;

	template <typename T>
	concept DurationType = IsSame<T, std::chrono::nanoseconds>
		|| IsSame<T, std::chrono::microseconds>
		|| IsSame<T, std::chrono::milliseconds>
		|| IsSame<T, std::chrono::seconds>;

	class TimerBase
	{
	public:
		constexpr TimerBase() = default;
		virtual ~TimerBase() = default;

		[[nodiscard]] inline bool IsRunning() const noexcept { return m_isRunning; }

		template<DurationType T = std::chrono::milliseconds>
		[[nodiscard]] f64 Elapsed() const noexcept
		{
			auto elapsed = m_isRunning ? (Clock::now() - m_startTime + m_accumulated) : m_accumulated;
			return std::chrono::duration<f64, typename T::period>(elapsed).count();
		}
		
		template<DurationType T = std::chrono::milliseconds>
		[[nodiscard]] T ElapsedDuration() const noexcept
		{
			auto elapsed = m_isRunning ? (Clock::now() - m_startTime + m_accumulated) : m_accumulated;
			return std::chrono::duration_cast<T>(elapsed);
		}

	protected:
		TimePoint m_startTime{};
		Duration  m_accumulated{};
		bool      m_isRunning{ false };
	};
}
