#pragma once

#define RUY_EXPORT __declspec(dllexport)
#define RUY_IMPORT __declspec(dllimport)

#if defined(RYU_EXPORTS)
#ifndef RYU_API
#define RYU_API RUY_EXPORT
#endif
#else
#ifndef RYU_API
#define RYU_API
#endif
#endif
