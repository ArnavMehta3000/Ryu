#pragma once
#include <tracy/Tracy.hpp>

#if defined(RYU_PROFILING_ENABLED)

// Check if tracy is enabled
#define RYU_PROFILER_CHECK() TracyNoop
#define RYU_PROFILER_SET_APPNAME(name) TracySetProgramName(name)

#define RYU_PROFILE_SCOPE() ZoneScoped
#define RYU_PROFILE_SCOPEN(name) ZoneScopedN(name)
#define RYU_PROFILE_SCOPENC(name, color) ZoneScopedNC(name, color)

#define RYU_PROFILE_SCOPE_NAME(str) ZoneName(str.c_str(), str.size())
#define RYU_PROFILE_SCOPE_TEXT(str) ZoneText(str.c_str(), str.size())

#define RYU_PROFILE_SCOPE_VALUE(value) ZoneValue(value)

#define RYU_PROFILE_BOOKMARK(name) TracyMessageL(name)
#define RYU_PROFILE_BOOKMARKC(name, color) TracyMessageLC(name, color)
#define RYU_PROFILE_BOOKMARK_STR(name, size) TracyMessage(name, size)
#define RYU_PROFILE_BOOKMARKC_STR(name, size, color) TracyMessageC(name, size, color)

#define RYU_PROFILE_MARK_FRAME() FrameMark

#define RYU_PROFILE_CONFIGURE_PLOT(Name, Type, Step, Fill, Color) TracyPlotConfig(Name, Type, Step, Fill, Color)
#define RYU_PROFILE_PLOT_VALUE(Name, Value) TracyPlot(Name, Value)
#else
#define RYU_PROFILER_CHECK() false
#define RYU_PROFILER_SET_APPNAME(name)

#define RYU_PROFILE_SCOPE()
#define RYU_PROFILE_SCOPEN(name)
#define RYU_PROFILE_SCOPENC(name, color)

#define RYU_PROFILE_SCOPE_NAME(str)
#define RYU_PROFILE_SCOPE_TEXT(str)

#define RYU_PROFILE_SCOPE_VALUE(value)

#define RYU_PROFILE_BOOKMARK(name)
#define RYU_PROFILE_BOOKMARKC(name, color)
#define RYU_PROFILE_BOOKMARK_STR(name, size)
#define RYU_PROFILE_BOOKMARKC_STR(name, size, color)

#define RYU_PROFILE_MARK_FRAME()

#define RYU_PROFILE_CONFIGURE_PLOT(Name, Type, Step, Fill, Color)
#define RYU_PROFILE_PLOT_VALUE(Name, Value)
#endif 

