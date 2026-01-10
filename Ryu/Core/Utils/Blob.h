#pragma once

namespace Ryu::Utils
{
	struct Blob
	{
		void* Data{ nullptr };
		u64 Size{ 0 };

		[[nodiscard]] inline bool IsValid() const { return Data != nullptr; }
		inline void Clear() { Data = nullptr; Size = 0; }

		inline operator bool() const { return IsValid(); }
	};
}
