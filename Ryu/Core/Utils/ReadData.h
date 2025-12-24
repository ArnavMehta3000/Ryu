#pragma once
#include "Core/Common/StandardTypes.h"
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <vector>
#include <optional>
#include <filesystem>

namespace Ryu::Utils
{
	namespace fs = std::filesystem;

    using ReadDataResult = std::optional<std::vector<byte>>;

    inline ReadDataResult ReadData(const fs::path& name)
    {
        std::ifstream inFile(name, std::ios::in | std::ios::binary | std::ios::ate);

        if (!inFile)
        {
        	return std::nullopt;
        }

        const std::streampos len = inFile.tellg();
        if (!inFile)
        {
        	return std::nullopt;
        }

        std::vector<byte> blob;
        blob.resize(size_t(len));

        inFile.seekg(0, std::ios::beg);
        if (!inFile)
        {
        	return std::nullopt;
        }

        inFile.read(reinterpret_cast<char*>(blob.data()), len);
        if (!inFile)
        {
        	return std::nullopt;
        }

        inFile.close();

        return blob;
    }
}
