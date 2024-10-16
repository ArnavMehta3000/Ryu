#include "Common.h"
#include "Utils/StringConv.h"

#ifndef RYU_BREAK_ON_FAIL_HRESULT
#define RYU_BREAK_ON_FAIL_HRESULT 0
#endif


void Ryu::Graphics::Internal::LogHResultError(HRESULT hr, std::string_view functionName, std::string_view fileName, u32 lineNumber)
{
	const std::string err = Utils::HRToString(hr);

#if RYU_BREAK_ON_FAIL_HRESULT
	LOG_FATAL(USE_LOG_CATEGORY(GraphicsPanic), "HRESULT failed {}({}):{} - {} ", fileName, lineNumber, functionName, err);
#else
	LOG_ERROR(USE_LOG_CATEGORY(GraphicsPanic), "HRESULT failed {}({}):{} - {} ", fileName, lineNumber, functionName, err);
#endif
}
