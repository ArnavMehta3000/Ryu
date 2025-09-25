#pragma once
#include "Common/StandardTypes.h"
#include <Elos/Utils/Timer.h>
#include <chrono>

namespace Ryu::Utils
{
	using Timer       = Elos::Timer;
	using TimeInfo    = Elos::Timer::TimeInfo;
	using TimePoint   = Elos::Timer::TimePoint;
	using ScopedTimer = Elos::ScopedTimer;
}

namespace Ryu::Utils::Experimental
{
	template <typename Clock = std::chrono::steady_clock, typename Duration = std::chrono::microseconds>
	class Timer
	{
		using timePoint = typename Clock::time_point;

	public:
		Timer() : m_t0(Clock::now()) { m_t1 = m_t0; }

		typename Duration::rep Mark()
		{
			TimePoint t2 = Clock::now();
			auto dt = std::chrono::duration_cast<Duration>(t2 - m_t1).count();
			m_t1 = t2;
			return dt;
		}

		typename Duration::rep Peek() const
		{
			tp t2 = Clock::now();
			auto dt = std::chrono::duration_cast<Duration>(t2 - m_t1).count();
			return dt;
		}

		typename Duration::rep Elapsed() const
		{
			tp t2 = Clock::now();
			auto elapsed = std::chrono::duration_cast<Duration>(t2 - m_t0).count();
			return elapsed;
		}

		f32 ElapsedInSeconds() const
		{
			return Elapsed() / s_durationSecondRatio;
		}

		f32 PeekInSeconds() const
		{
			return Peek() / s_durationSecondRatio;
		}

		f32 MarkInSeconds()
		{
			return Mark() / s_durationSecondRatio;
		}

	private:
		static constexpr f32 s_durationSecondRatio = std::chrono::seconds(1) * 1.0f / Duration(1);
		const TimePoint m_t0;
		TimePoint m_t1;
	};
}
