#pragma once
#include "Utils/Singleton.h"
#include "Config/CVar.h"
#include <CLI/CLI.hpp>
#include <unordered_map>
#include <vector>
#include <memory>
#include <filesystem>

namespace Ryu::Config
{
    namespace fs = std::filesystem;

	class CmdLine final : public Utils::Singleton<CmdLine>
	{
        RYU_SINGLETON_DECLARE(CmdLine);
	public:
        ~CmdLine() = default;
        template<Internal::CVarAllTypes T>
        void RegisterCVar(std::string_view name, CVar<T>* cvar);

        template<Internal::CVarAllTypes T>
        void AddToCLI(std::string_view name, CVar<T>* cvar);

        ICVarBase* FindCVar(std::string_view name) const;
        std::string GetCVarValue(std::string_view name) const;
        bool SetCVarValue(std::string_view name, std::string_view value);

        bool ParseCommandLine();
        void SaveFileConfig(const fs::path& filename);

        const inline NODISCARD auto& GetAllCVars() const { return m_cvars; }
        const inline NODISCARD auto& GetCLIApp() const { return *m_cliApp; }

    private:
        CmdLine() = default;

    private:
        std::unordered_map<std::string, ICVarBase*> m_cvars;
        std::unique_ptr<CLI::App> m_cliApp;

        void EnsureCLIApp();
	};
}

#include "Config/CmdLine.inl"
