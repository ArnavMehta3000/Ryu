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
		DLL(const DLL&) = delete;
		DLL(DLL&&) noexcept = default;
		~DLL();

		bool Load(std::filesystem::path libPath);
		void Unload();
		bool IsLoaded();

		DLLFunc GetSymbol(std::string_view name) const;

		DLL& operator=(const DLL&) = delete;
		DLL& operator=(DLL&& lib) noexcept = default;
	
	private:
		HMODULE m_handle;
	};
}
