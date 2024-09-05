#pragma once
#include <Core/StandardTypes.h>
#include <string>
#include <vector>
#include <Windows.h>

namespace Ryu
{
	class RYU_API Caption
	{
	public:
		static constexpr u32 CLOSE    = 0;
		static constexpr u32 MINIMIZE = 1;
		static constexpr u32 MAXIMIZE = 2;
		static constexpr u32 TIMER_ID = 101;

	public:
		struct Button
		{
			std::wstring Text = L"";
			i32 Command = -1;
			i32 Width = 50;
			RECT Rect;

			explicit Button(const std::wstring& text, i32 command, i32 width = 50)
				: Text(text), Command(command), Width(width), Rect()
			{}
		};

	public:
		Caption() = default;
		~Caption() = default;

		inline bool CanShowTitle() const { return m_showTitle; }
		inline void ShowTitle(bool show) { m_showTitle = show; }
		inline void AddButton(const Button& button) { m_buttons.push_back(button); }
		inline std::vector<Button>& GetButtons() { return m_buttons; }

	private:
		bool m_showTitle{ true };
		std::vector<Caption::Button> m_buttons;
	};
}