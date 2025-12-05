module;

export module Ryu.Core.Global;

// TODO: Sort out the forward declaration stuff once utils is done
//namespace Ryu::Utils { class ServiceLocator; }

export namespace Ryu::Globals
{
    extern const bool g_isDebug;

	extern bool IsDebuggerAttached();

	//extern Utils::ServiceLocator& GetServiceLocator();
}