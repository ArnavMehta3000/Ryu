module;
#include <Elos/Utils/Timer.h>
#include <chrono>

export module Ryu.Core.Utils:Timer;

export namespace Ryu::Utils
{
	using Timer       = Elos::Timer;
	using TimeInfo    = Elos::Timer::TimeInfo;
	using TimePoint   = Elos::Timer::TimePoint;
	using ScopedTimer = Elos::ScopedTimer;
}
