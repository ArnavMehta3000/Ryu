#pragma once
#include "Common/ObjectMacros.h"
#include "Config/ConfigBase.h"
#include "Config/ConfigManager.h"// Macro to help with declaring configs

#define RYU_DECLARE_CONFIG(ThisType)                                                                    \
	public:                                                                                             \
		static ThisType& Get() { return ::Ryu::Utils::Singleton<ThisType>::Get(); }                     \
	private:                                                                                            \
		friend class ::Ryu::Utils::Singleton<ThisType>;                                                 \
		friend class ::Ryu::Config::ConfigBase;                                                         \
		friend class ::Ryu::Config::ConfigManager;                                                      \
	public:                                                                                             \
		virtual ~ThisType() {}                                                                          \
		static inline const struct ConfigRegistrar                                                      \
		{                                                                                               \
			::Elos::Connection<bool> m_onInitializedConnection;                                         \
			ConfigRegistrar()                                                                           \
			{                                                                                           \
				m_onInitializedConnection = ::Ryu::Config::ConfigManager::Get().OnInitialized.Connect(  \
					[](bool) {                                                                          \
						::Ryu::Config::ConfigManager::Get().RegisterConfig(&Get());                     \
						Get().Load();                                                                   \
					}                                                                                   \
				);                                                                                      \
			}                                                                                           \
	} s_registrar {}

#define RYU_DECLARE_CONFIG_SECTION(Name) static constexpr::Ryu::Config::ConfigSection RYU_CONCAT(Name, Section) = #Name
