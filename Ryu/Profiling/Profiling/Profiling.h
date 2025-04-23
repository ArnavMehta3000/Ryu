#pragma once

#if defined(RYU_PROFILING_ENABLED)
#include <tracy/Tracy.hpp>

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
#define RYU_PROFILER_SET_APPNAME(name) void(0)

#define RYU_PROFILE_SCOPE() void(0)
#define RYU_PROFILE_SCOPEN(name) void(0)
#define RYU_PROFILE_SCOPENC(name, color) void(0)

#define RYU_PROFILE_SCOPE_NAME(str) void(0)
#define RYU_PROFILE_SCOPE_TEXT(str) void(0)

#define RYU_PROFILE_SCOPE_VALUE(value) void(0)

#define RYU_PROFILE_BOOKMARK(name) void(0)
#define RYU_PROFILE_BOOKMARKC(name, color) void(0)
#define RYU_PROFILE_BOOKMARK_STR(name, size) void(0)
#define RYU_PROFILE_BOOKMARKC_STR(name, size, color) void(0)

#define RYU_PROFILE_MARK_FRAME() void(0)

#define RYU_PROFILE_CONFIGURE_PLOT(Name, Type, Step, Fill, Color)
#define RYU_PROFILE_PLOT_VALUE(Name, Value) void(0)
#endif 

