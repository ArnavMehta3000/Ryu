#pragma once

// These are the fonts available for use with ImGui
extern "C"
{
	extern const u8 _binary_MapleMono_ttf_start[];
	extern const u8 _binary_MapleMono_ttf_end[];

	extern const u8 _binary_Roboto_ttf_start[];
	extern const u8 _binary_Roboto_ttf_end[];

	extern const u8 _binary_Hurmit_otf_start[];
	extern const u8 _binary_Hurmit_otf_end[];
}

struct ImFont;

namespace Ryu::Editor::Font
{
	enum class Style
	{
		MapleMono,
		Roboto,
		Hurmit,
	};

	std::array<ImFont*, 3> g_editorFonts{ nullptr, nullptr, nullptr };

	inline constexpr void* GetFontData(Style style)
	{
		switch (style)
		{
			using enum Style;
		case MapleMono: return (void*)_binary_MapleMono_ttf_start;
		case Roboto   : return (void*)_binary_Roboto_ttf_start;
		case Hurmit   : return (void*)_binary_Hurmit_otf_start;
		default       : return nullptr;
		}
	}

	inline constexpr u32 GetFontDataSize(Style style)
	{
#define U32(X) static_cast<u32>((X))
		switch (style)
		{
			using enum Style;
		case MapleMono: return U32(_binary_MapleMono_ttf_end - _binary_MapleMono_ttf_start);
		case Roboto   : return U32(_binary_Roboto_ttf_end - _binary_Roboto_ttf_start);
		case Hurmit   : return U32(_binary_Hurmit_otf_end - _binary_Hurmit_otf_start);
		default       : return 0;
		}
#undef U32
	}
}
