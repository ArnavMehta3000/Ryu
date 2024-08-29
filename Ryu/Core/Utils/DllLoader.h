#pragma once
#include <CoreDLLDefines.h>
#include <Core/ObjectMacros.h>
#include <Core/Concepts.h>
#include <Windows.h>
#include <unordered_map>
#include <string_view>

namespace Ryu
{
	template <typename T>
	class RYU_API DllPointer
	{
	public:
		using ValueType = T;
		using PointerType = T*;

		DllPointer() = default;
		DllPointer(PointerType ptr)
			: m_ptr(ptr)
		{}

		auto Get() const { return m_ptr; }
		auto operator->() const { return m_ptr; }
		operator bool() const { return m_ptr != nullptr; }

	private:
		PointerType m_ptr{ nullptr };
	};

	template <typename T>
	concept IsDllPointer = IsSame<DllPointer<T>, T>;

	class RYU_API DllLoader
	{
	public:
		~DllLoader();
		bool LoadDLL(std::string_view dllName);
		bool LoadProcAddress(std::string_view funcName);

		void UnloadDLL();

		template <typename T> requires IsFuncPtr<T> && (!IsDllPointer<T>)
		T GetProcAddressFunc(std::string_view funcName)
		{
			if (m_funcMap.find(funcName.data()) == m_funcMap.end())
			{
				// Proc address not found. Try loading it
				if (LoadProcAddress(funcName)) 
				{
					// Proc address loaded. Return it
					return reinterpret_cast<T>(m_funcMap[funcName.data()]);
				}
				return nullptr;
			}
			
			return reinterpret_cast<T>(m_funcMap[funcName.data()]);
		}

		template <typename T> requires (!IsPtr<T>)
		DllPointer<T> GetProcAddressPtr(std::string ptrName)
		{
			if (m_funcMap.find(ptrName.data()) == m_funcMap.end())
			{
				// Proc address not found. Try loading it
				if (LoadProcAddress(ptrName))
				{
					// Proc address loaded. Return it
					FARPROC address = m_funcMap[ptrName.data()];
					return DllPointer<T>(*reinterpret_cast<typename DllPointer<T>::PointerType*>(address));
				}
				return DllPointer<T>();
			}

			FARPROC address = m_funcMap[ptrName.data()];
			return DllPointer<T>(*reinterpret_cast<typename DllPointer<T>::PointerType*>(address));
		}

		inline HMODULE GetHandle() const noexcept { return m_hModule; }


	private:
		HMODULE m_hModule{ nullptr };
		std::unordered_map<std::string, FARPROC> m_funcMap;
	};	
}