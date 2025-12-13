#pragma once
#include "Common/StandardTypes.h"
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <vector>
#include <optional>

namespace Ryu::Utils
{
    inline std::optional<std::vector<byte>> ReadData(const wchar_t* name)
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

        inFile.read(reinterpret_cast<byte*>(blob.data()), len);
        if (!inFile)
        {
        	return std::nullopt;
        }

        inFile.close();

        return blob;
    }
}
