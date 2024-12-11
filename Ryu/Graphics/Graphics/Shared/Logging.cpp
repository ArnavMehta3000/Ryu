#include "Logging.h"
#include "Utils/StringConv.h"
#include <d3d12.h>

// RYU_BREAK_ON_FAIL_HRESULT
// - Configured via xmake

namespace Ryu::Graphics::Internal
{
	void Ryu::Graphics::Internal::LogHResultError(long hr, std::string_view functionName, std::string_view fileName, unsigned int lineNumber)
	{
		const std::string err = Utils::HRToString(hr);

#if defined(RYU_BREAK_ON_FAIL_HRESULT)
		LOG_FATAL(RYU_USE_LOG_CATEGORY(GraphicsPanic), "HRESULT failed {}({}):{} - {} ", fileName, lineNumber, functionName, err);
#else
		LOG_ERROR(RYU_USE_LOG_CATEGORY(GraphicsPanic), "HRESULT failed {}({}):{} - {} ", fileName, lineNumber, functionName, err);
#endif
	}

	void Ryu::Graphics::Internal::SetNameDX12(ID3D12Object* obj, std::wstring name)
	{
		if (obj != nullptr)
		{
			LOG_DEBUG(RYU_USE_LOG_CATEGORY(GraphicsDebug), "DX12 object named: {}", Utils::ToNarrowStr(name));
			obj->SetName(name.c_str());
		}
	}

	void Ryu::Graphics::Internal::LogDX11Naming(std::string_view name)
	{
		LOG_DEBUG(RYU_USE_LOG_CATEGORY(GraphicsDebug), "DX11 object named: {}", name);
	}
}
