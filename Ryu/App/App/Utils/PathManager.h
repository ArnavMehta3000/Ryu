#pragma  once
#include "Common/ObjectMacros.h"
#include "Common/API.h"
#include "Utils/Singleton.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace Ryu::App
{
	class RYU_API PathManager : public Utils::Singleton<PathManager>
	{
		RYU_SINGLETON_DECLARE(PathManager);

	public:
		~PathManager() = default;
		void Init();

		inline NODISCARD const fs::path& GetRootDir() const { return m_rootDir; }
		inline NODISCARD const fs::path& GetProjectDir() const { return m_projectDir; }
		inline NODISCARD const fs::path& GetGameDLLPath() const { return m_gameDLLPath; }
		inline NODISCARD const std::string& GetGameDLLName() const { return m_gameDLLName; }

	private:
		PathManager() = default;
		fs::path FindRootDirectory();
		void LoadConfig();

	private:
		fs::path    m_rootDir;
		fs::path    m_projectDir;
		fs::path    m_gameDLLPath;
		std::string m_gameDLLName;
	};
}
