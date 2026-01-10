#pragma once

namespace Ryu::Editor::Theme
{
	enum class Style
	{
		DefaultDark = 0,
		DefaultLight,
		Classic,
		SimpleDark,
		DeepDark,
		CatpuccinMocha,
		FluentDark,
		Dracula,

		MAX_COUNT
	};

	void Set(Style theme);
}
