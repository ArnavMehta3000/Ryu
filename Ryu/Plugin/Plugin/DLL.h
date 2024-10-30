#pragma once
#include "Common/Common.h"
#include <Windows.h>

namespace std::filesystem { class path; }

namespace Ryu::Plugin
{
	// Generic function pointer
	using DLLFunc = void(*)(void);

	class DLL
	{
	public:
		DLL();
		~DLL();

		bool Load(std::filesystem::path libPath);
		void Unload();
		bool IsLoaded();

		DLLFunc GetSymbol(std::string_view name) const;
	
	private:
		HMODULE m_handle;
	};
}
