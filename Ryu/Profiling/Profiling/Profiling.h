#pragma once
#include <tracy/Tracy.hpp>

#define RYU_PROFILE_SCOPE() ZoneScoped
#define RYU_PROFILE_SCOPEN(name) ZoneScopedN(name)
#define RYU_PROFILE_SCOPENC(name, color) ZoneScopedNC(name, color)

#define RYU_PROFILE_MARK_FRAME() FrameMark
