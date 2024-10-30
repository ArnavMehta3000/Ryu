#include "DLL.h"
#include <filesystem>
#include <libassert/assert.hpp>

namespace Ryu::Plugin
{
	DLL::DLL()
		:m_handle(nullptr)
	{
	}
	
	DLL::~DLL()
	{
		Unload();
	}
	
	bool DLL::Load(std::filesystem::path libPath)
	{
		m_handle = LoadLibraryExW(libPath.wstring().c_str(), nullptr, 
			libPath.is_absolute() ? LOAD_WITH_ALTERED_SEARCH_PATH : 0);

		return m_handle != nullptr;
	}
	
	void DLL::Unload()
	{
		if (m_handle)
		{
			::FreeLibrary(m_handle);
		}
	}
	
	bool DLL::IsLoaded()
	{
		return m_handle != nullptr;
	}
		
	DLLFunc DLL::GetSymbol(std::string_view name) const
	{
		ASSERT(m_handle, "Library is not loaded");
		return reinterpret_cast<DLLFunc>(GetProcAddress(m_handle, name.data()));
	}
}
