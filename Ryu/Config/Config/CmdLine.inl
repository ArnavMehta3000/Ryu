#if defined (RYU_BUILD_DEBUG)
#include <cassert>
#include "CmdLine.h"
#endif

namespace Ryu::Config
{
	template<Internal::CVarAllTypes T>
	constexpr inline CVar<T>::CVar(std::string_view name, const T defaultValue, std::string_view description, CVarFlags flags, CallbackType callback)
		: m_name(name)
		, m_description(description)
		, m_value(defaultValue)
		, m_cliValue(std::nullopt)
		, m_flags(flags)
		, m_callback(std::move(callback))
	{
#if defined (RYU_BUILD_DEBUG)
		assert(!name.empty());
#endif

		CmdLine::Get().RegisterCVar(name, this);  // Automatically register
	}

	template<Internal::CVarAllTypes  T>
	inline void CmdLine::RegisterCVar(std::string_view name, CVar<T>* cvar)
	{
		if (!cvar)
		{
			return;
		}

		const std::string nameStr(name);
		if (m_cvars.contains(nameStr))
		{
			throw std::runtime_error("CVar with same name found!");
		}

		m_cvars[nameStr] = std::unique_ptr<ICVarBase>(cvar);

		AddToCLI(name, cvar);
	}

	template<Internal::CVarAllTypes  T>
	inline void Config::CmdLine::AddToCLI(std::string_view name, CVar<T>* cvar)
	{
		EnsureCLIApp();

		std::string cliName = std::format("--{}", name);
		std::string description(cvar->GetDescription());

		// Add flags info to description (TODO: move this to enum string autogen)
		if (cvar->GetFlags() != CVarFlags::None)
		{
			description += " [";
			bool first = true;
			if ((cvar->GetFlags() & CVarFlags::ReadOnly) != CVarFlags::None)
			{
				if (!first) description += ", ";
				description += "ReadOnly";
				first = false;
			}
			if ((cvar->GetFlags() & CVarFlags::Debug) != CVarFlags::None)
			{
				if (!first) description += ", ";
				description += "Debug";
				first = false;
			}
			description += "]";
		}

		// Add vector-specific info to description
		if constexpr (Internal::CVarVectorType<T>)
		{
			description += " (comma-separated values)";
		}

		try
		{
			CLI::Option* option = m_cliApp->add_option(cliName, cvar->m_cliValue, description);
			option->ignore_case();
			
			// Configure CLI11 option for vectors
			if constexpr (Internal::CVarVectorType<T>)
			{
				// Allow multiple values and set delimiter
				option->delimiter(',');

				// TODO: Somehow configure min required args (eg dimensions has to have two)
			}
		}
		catch (const std::exception& e)
		{
			OutputDebugStringA(e.what());
		}
	}
}
