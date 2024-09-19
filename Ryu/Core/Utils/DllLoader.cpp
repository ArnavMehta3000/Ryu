#include "DLLLoader.h"

namespace Ryu
{
	DllLoader::~DllLoader()
	{
		UnloadDLL();
	}

	bool DllLoader::LoadDLL(std::string_view dllName)
	{
		m_hModule = ::LoadLibraryA(dllName.data());

		if (!m_hModule)
		{
			std::string message = "Failed to load " + std::string(dllName) + " Error Code: " + std::to_string(::GetLastError());
			::MessageBoxA(nullptr, message.c_str(), "Error", MB_OK | MB_ICONERROR);
			return false;
		}

		return true;
	}

	bool DllLoader::LoadProcAddress(std::string_view funcName)
	{
		if (!m_hModule)
			return false;

		FARPROC proc = ::GetProcAddress(m_hModule, funcName.data());
		if (!proc)
			return false;

		m_dataMap[std::string(funcName)] = proc;
		return true;
	}

	void DllLoader::UnloadDLL()
	{
		if (m_hModule)
		{
			::FreeLibrary(m_hModule);
			m_hModule = nullptr;
		}
	}
}
