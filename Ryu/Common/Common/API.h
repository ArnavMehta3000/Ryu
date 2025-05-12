#pragma once

#if defined(RYU_EXPORTS)
#ifndef RYU_API
#define RYU_API __declspec(dllexport)
#endif
#else
#ifndef RYU_API
#define RYU_API
#endif
#endif
