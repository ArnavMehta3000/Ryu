#pragma once
#include "Common/Common.h"
#include <External/AngelScript/AddOns/scriptbuilder/scriptbuilder.h>

namespace Ryu::Scripting
{
	/**
	 * @brief Reference counted scriptable object that can be used with AngelScript
	 */
	class IScriptableObject
	{
	public:
		IScriptableObject();
		virtual ~IScriptableObject();

		constexpr inline i32 AddRef() noexcept { return ++m_refCount; }
		constexpr inline i32 Release() noexcept { return --m_refCount; }
		constexpr inline i32 GetRefCount() const noexcept { return m_refCount; }
		constexpr inline std::string_view GetFriendlyName() const { return m_friendlyName; }
		asILockableSharedBool* GetWeakRefFlag();

		virtual void CacheData(CScriptBuilder& builder);

	protected:
		std::string m_friendlyName;
		asIScriptModule* m_module;

	private:
		i32 m_refCount;
		asILockableSharedBool* m_weakRefFlag;
	};
}
