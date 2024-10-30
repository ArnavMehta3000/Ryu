#pragma once

#if defined(RYU_PLUGIN_API_EXPORTS)
#define RYU_PLUGIN_API __declspec(dllexport)
#else
#define RYU_PLUGIN_API __declspec(dllimport)
#endif
