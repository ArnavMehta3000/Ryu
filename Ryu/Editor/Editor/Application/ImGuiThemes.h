#pragma once

namespace Ryu::Editor
{
	enum class ImGuiTheme
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

	void SetTheme(ImGuiTheme theme);
}
