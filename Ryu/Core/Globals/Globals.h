#pragma once

namespace Ryu::Utils { class ServiceLocator; }

namespace Ryu::Globals
{
	extern const bool g_isDebug;

	extern bool IsDebuggerAttached();

	extern Utils::ServiceLocator& GetServiceLocator();
}
