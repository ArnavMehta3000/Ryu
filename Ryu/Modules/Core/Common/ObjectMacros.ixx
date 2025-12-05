module;
#include "Common/ObjectMacros.h"

export module Ryu.Core.Common:ObjectMacros;

export namespace Ryu
{
	constexpr std::string_view ExtractFilename(std::string_view path) noexcept
	{
		// Find the last occurrence of either '/' or '\'
		size_t lastSlash = path.find_last_of("/\\");

		// No slash found, return the entire string (it's already just a filename)
		if (lastSlash == std::string_view::npos)
		{
			return path;
		}

		// Return substring starting after the last slash
		return path.substr(lastSlash + 1);
	}

	constexpr std::string_view ExtractFilenameNoExt(std::string_view path) noexcept
	{
		std::string_view filename = ExtractFilename(path);

		// Find last dot
		size_t lastDot = filename.find_last_of('.');

		// No dot found, return the entire filename
		if (lastDot == std::string_view::npos)
		{
			return filename;
		}

		// Return substring up to (but not including) the last dot
		return filename.substr(0, lastDot);
	}
}