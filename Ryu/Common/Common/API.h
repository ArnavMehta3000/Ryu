#pragma once

#if defined(RYU_EXPORTS)
#define RYU_API __declspec(dllexport)
#else
#define RYU_API __declspec(dllimport)
#endif
