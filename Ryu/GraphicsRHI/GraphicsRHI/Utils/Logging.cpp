#include "Logging.h"
#include "Utils/StringConv.h"
#include <d3d12.h>
#include <format>

// RYU_BREAK_ON_FAIL_HRESULT
// - Configured via xmake

namespace Ryu::Graphics::Internal
{
	void LogHResultError(long hr, std::string_view functionName, std::string_view fileName, unsigned int lineNumber)
	{
		const std::string err = ::Ryu::Utils::HRToString(hr);

#if defined(RYU_BREAK_ON_FAIL_HRESULT)
		RYU_LOG_FATAL(RYU_LOG_USE_CATEGORY(GraphicsPanic), "HRESULT failed {}({}):{} - {} ", fileName, lineNumber, functionName, err);
#else
		RYU_LOG_ERROR(RYU_LOG_USE_CATEGORY(GraphicsPanic), "HRESULT failed {}({}):{} - {} ", fileName, lineNumber, functionName, err);
#endif
	}

	void SetNameDX12(ID3D12Object* obj, std::wstring name)
	{
		if (obj != nullptr)
		{
			RYU_LOG_DEBUG(RYU_LOG_USE_CATEGORY(GraphicsDebug), "DX12 object named: {}", ::Ryu::Utils::ToNarrowStr(name));
			obj->SetName(name.c_str());
		}
	}

	void LogDX11Naming(const std::string& msg)
	{
		RYU_LOG_DEBUG(RYU_LOG_USE_CATEGORY(GraphicsDebug), "{}", msg);
	}
}
