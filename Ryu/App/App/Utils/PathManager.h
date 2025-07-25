#pragma  once
#include "Common/ObjectMacros.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace Ryu::App
{
	class PathManager
	{
	public:
		PathManager();

		inline NODISCARD const fs::path& GetRootDir() const { return m_rootDir; }
		inline NODISCARD const fs::path& GetProjectDir() const { return m_projectDir; }

	private:
		fs::path FindRootDirectory();
		void LoadConfig();

	private:
		fs::path    m_rootDir;
		fs::path    m_projectDir;
	};
}
