#include "Editor/Application/ImGuiThemes.h"
#include <ImGui/imgui.h>

namespace Ryu::Editor::Theme
{
	static void SetThemeSimpleDark()
	{
		ImGuiStyle& style                            = ImGui::GetStyle();
		style.Colors[ImGuiCol_Text]                  = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
		style.Colors[ImGuiCol_ChildBg]               = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
		style.Colors[ImGuiCol_PopupBg]               = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
		style.Colors[ImGuiCol_Border]                = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
		style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
		style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
		style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
		style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
		style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
		style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
		style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.08f, 0.50f, 0.72f, 1.00f);
		style.Colors[ImGuiCol_Button]                = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
		style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
		style.Colors[ImGuiCol_Header]                = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
		style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
		style.Colors[ImGuiCol_Separator]             = style.Colors[ImGuiCol_Border];
		style.Colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.41f, 0.42f, 0.44f, 1.00f);
		style.Colors[ImGuiCol_SeparatorActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
		style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.29f, 0.30f, 0.31f, 0.67f);
		style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
		style.Colors[ImGuiCol_Tab]                   = ImVec4(0.08f, 0.08f, 0.09f, 0.83f);
		style.Colors[ImGuiCol_TabHovered]            = ImVec4(0.33f, 0.34f, 0.36f, 0.83f);
		style.Colors[ImGuiCol_TabActive]             = ImVec4(0.23f, 0.23f, 0.24f, 1.00f);
		style.Colors[ImGuiCol_TabUnfocused]          = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
		style.Colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
		style.Colors[ImGuiCol_DockingPreview]        = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
		style.Colors[ImGuiCol_DockingEmptyBg]        = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		style.Colors[ImGuiCol_DragDropTarget]        = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
		style.Colors[ImGuiCol_NavHighlight]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		style.Colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		style.Colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
		style.GrabRounding                           = style.FrameRounding = 2.3f;
	}

	static void SetThemeDeepDark()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.Colors[ImGuiCol_Text]                  = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		style.Colors[ImGuiCol_ChildBg]               = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_PopupBg]               = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
		style.Colors[ImGuiCol_Border]                = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
		style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
		style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
		style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
		style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		style.Colors[ImGuiCol_Button]                = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		style.Colors[ImGuiCol_Header]                = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
		style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
		style.Colors[ImGuiCol_Separator]             = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		style.Colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		style.Colors[ImGuiCol_SeparatorActive]       = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		style.Colors[ImGuiCol_Tab]                   = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		style.Colors[ImGuiCol_TabHovered]            = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		style.Colors[ImGuiCol_TabActive]             = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
		style.Colors[ImGuiCol_TabUnfocused]          = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		style.Colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		style.Colors[ImGuiCol_DockingPreview]        = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		style.Colors[ImGuiCol_DockingEmptyBg]        = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_PlotLines]             = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		style.Colors[ImGuiCol_TableBorderStrong]     = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		style.Colors[ImGuiCol_TableBorderLight]      = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		style.Colors[ImGuiCol_TableRowBg]            = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_TableRowBgAlt]         = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		style.Colors[ImGuiCol_DragDropTarget]        = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		style.Colors[ImGuiCol_NavHighlight]          = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
		style.Colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
		style.Colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

		style.WindowPadding     = ImVec2(8.00f, 8.00f);
		style.FramePadding      = ImVec2(5.00f, 2.00f);
		style.CellPadding       = ImVec2(6.00f, 6.00f);
		style.ItemSpacing       = ImVec2(6.00f, 6.00f);
		style.ItemInnerSpacing  = ImVec2(6.00f, 6.00f);
		style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
		style.IndentSpacing     = 25;
		style.ScrollbarSize     = 15;
		style.GrabMinSize       = 10;
		style.WindowBorderSize  = 1;
		style.ChildBorderSize   = 1;
		style.PopupBorderSize   = 1;
		style.FrameBorderSize   = 1;
		style.TabBorderSize     = 1;
		style.WindowRounding    = 7;
		style.ChildRounding     = 4;
		style.FrameRounding     = 3;
		style.PopupRounding     = 4;
		style.ScrollbarRounding = 9;
		style.GrabRounding      = 3;
		style.LogSliderDeadzone = 4;
		style.TabRounding       = 4;
	}

	static void SetThemeCatpuccinMocha()
	{
		constexpr ImVec4 base     = ImVec4(0.117f, 0.117f, 0.172f, 1.0f); // #1e1e2e
		constexpr ImVec4 mantle   = ImVec4(0.109f, 0.109f, 0.156f, 1.0f); // #181825
		constexpr ImVec4 surface0 = ImVec4(0.200f, 0.207f, 0.286f, 1.0f); // #313244
		constexpr ImVec4 surface1 = ImVec4(0.247f, 0.254f, 0.337f, 1.0f); // #3f4056
		constexpr ImVec4 surface2 = ImVec4(0.290f, 0.301f, 0.388f, 1.0f); // #4a4d63
		constexpr ImVec4 overlay0 = ImVec4(0.396f, 0.403f, 0.486f, 1.0f); // #65677c
		constexpr ImVec4 overlay2 = ImVec4(0.576f, 0.584f, 0.654f, 1.0f); // #9399b2
		constexpr ImVec4 text     = ImVec4(0.803f, 0.815f, 0.878f, 1.0f); // #cdd6f4
		constexpr ImVec4 subtext0 = ImVec4(0.639f, 0.658f, 0.764f, 1.0f); // #a3a8c3
		constexpr ImVec4 mauve    = ImVec4(0.796f, 0.698f, 0.972f, 1.0f); // #cba6f7
		constexpr ImVec4 peach    = ImVec4(0.980f, 0.709f, 0.572f, 1.0f); // #fab387
		constexpr ImVec4 yellow   = ImVec4(0.980f, 0.913f, 0.596f, 1.0f); // #f9e2af
		constexpr ImVec4 green    = ImVec4(0.650f, 0.890f, 0.631f, 1.0f); // #a6e3a1
		constexpr ImVec4 teal     = ImVec4(0.580f, 0.886f, 0.819f, 1.0f); // #94e2d5
		constexpr ImVec4 sapphire = ImVec4(0.458f, 0.784f, 0.878f, 1.0f); // #74c7ec
		constexpr ImVec4 blue     = ImVec4(0.533f, 0.698f, 0.976f, 1.0f); // #89b4fa
		constexpr ImVec4 lavender = ImVec4(0.709f, 0.764f, 0.980f, 1.0f); // #b4befe

		ImGuiStyle& style = ImGui::GetStyle();

		style.Colors[ImGuiCol_WindowBg]              = base;
		style.Colors[ImGuiCol_ChildBg]               = base;
		style.Colors[ImGuiCol_PopupBg]               = surface0;
		style.Colors[ImGuiCol_Border]                = surface1;
		style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_FrameBg]               = surface0;
		style.Colors[ImGuiCol_FrameBgHovered]        = surface1;
		style.Colors[ImGuiCol_FrameBgActive]         = surface2;
		style.Colors[ImGuiCol_TitleBg]               = mantle;
		style.Colors[ImGuiCol_TitleBgActive]         = surface0;
		style.Colors[ImGuiCol_TitleBgCollapsed]      = mantle;
		style.Colors[ImGuiCol_MenuBarBg]             = mantle;
		style.Colors[ImGuiCol_ScrollbarBg]           = surface0;
		style.Colors[ImGuiCol_ScrollbarGrab]         = surface2;
		style.Colors[ImGuiCol_ScrollbarGrabHovered]  = overlay0;
		style.Colors[ImGuiCol_ScrollbarGrabActive]   = overlay2;
		style.Colors[ImGuiCol_CheckMark]             = green;
		style.Colors[ImGuiCol_SliderGrab]            = sapphire;
		style.Colors[ImGuiCol_SliderGrabActive]      = blue;
		style.Colors[ImGuiCol_Button]                = surface0;
		style.Colors[ImGuiCol_ButtonHovered]         = surface1;
		style.Colors[ImGuiCol_ButtonActive]          = surface2;
		style.Colors[ImGuiCol_Header]                = surface0;
		style.Colors[ImGuiCol_HeaderHovered]         = surface1;
		style.Colors[ImGuiCol_HeaderActive]          = surface2;
		style.Colors[ImGuiCol_Separator]             = surface1;
		style.Colors[ImGuiCol_SeparatorHovered]      = mauve;
		style.Colors[ImGuiCol_SeparatorActive]       = mauve;
		style.Colors[ImGuiCol_ResizeGrip]            = surface2;
		style.Colors[ImGuiCol_ResizeGripHovered]     = mauve;
		style.Colors[ImGuiCol_ResizeGripActive]      = mauve;
		style.Colors[ImGuiCol_Tab]                   = surface0;
		style.Colors[ImGuiCol_TabHovered]            = surface2;
		style.Colors[ImGuiCol_TabActive]             = surface1;
		style.Colors[ImGuiCol_TabUnfocused]          = surface0;
		style.Colors[ImGuiCol_TabUnfocusedActive]    = surface1;
		style.Colors[ImGuiCol_DockingPreview]        = sapphire;
		style.Colors[ImGuiCol_DockingEmptyBg]        = base;
		style.Colors[ImGuiCol_PlotLines]             = blue;
		style.Colors[ImGuiCol_PlotLinesHovered]      = peach;
		style.Colors[ImGuiCol_PlotHistogram]         = teal;
		style.Colors[ImGuiCol_PlotHistogramHovered]  = green;
		style.Colors[ImGuiCol_TableHeaderBg]         = surface0;
		style.Colors[ImGuiCol_TableBorderStrong]     = surface1;
		style.Colors[ImGuiCol_TableBorderLight]      = surface0;
		style.Colors[ImGuiCol_TableRowBg]            = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_TableRowBgAlt]         = ImVec4(1.0f, 1.0f, 1.0f, 0.06f);
		style.Colors[ImGuiCol_TextSelectedBg]        = surface2;
		style.Colors[ImGuiCol_DragDropTarget]        = yellow;
		style.Colors[ImGuiCol_NavHighlight]          = lavender;
		style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
		style.Colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.8f, 0.8f, 0.8f, 0.2f);
		style.Colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.0f, 0.0f, 0.0f, 0.35f);
		style.Colors[ImGuiCol_Text]                  = text;
		style.Colors[ImGuiCol_TextDisabled]          = subtext0;

		style.WindowRounding    = 6.0f;
		style.ChildRounding     = 6.0f;
		style.FrameRounding     = 4.0f;
		style.PopupRounding     = 4.0f;
		style.ScrollbarRounding = 9.0f;
		style.GrabRounding      = 4.0f;
		style.TabRounding       = 4.0f;

		style.WindowPadding    = ImVec2(8.0f, 8.0f);
		style.FramePadding     = ImVec2(5.0f, 3.0f);
		style.ItemSpacing      = ImVec2(8.0f, 4.0f);
		style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
		style.IndentSpacing    = 21.0f;
		style.ScrollbarSize    = 14.0f;
		style.GrabMinSize      = 10.0f;

		style.WindowBorderSize = 1.0f;
		style.ChildBorderSize  = 1.0f;
		style.PopupBorderSize  = 1.0f;
		style.FrameBorderSize  = 0.0f;
		style.TabBorderSize    = 0.0f;
	}

	static void SetThemeFluentDark()
	{
		ImGuiStyle& style = ImGui::GetStyle();

		style.WindowRounding    = 5.0f;
		style.FrameRounding     = 5.0f;
		style.ScrollbarRounding = 5.0f;
		style.GrabRounding      = 5.0f;
		style.TabRounding       = 5.0f;
		style.WindowBorderSize  = 1.0f;
		style.FrameBorderSize   = 1.0f;
		style.PopupBorderSize   = 1.0f;
		style.PopupRounding     = 5.0f;

		style.Colors[ImGuiCol_Text]                 = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
		style.Colors[ImGuiCol_TextDisabled]         = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
		style.Colors[ImGuiCol_WindowBg]             = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
		style.Colors[ImGuiCol_ChildBg]              = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		style.Colors[ImGuiCol_PopupBg]              = ImVec4(0.18f, 0.18f, 0.18f, 1.f);
		style.Colors[ImGuiCol_Border]               = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
		style.Colors[ImGuiCol_BorderShadow]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_FrameBg]              = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		style.Colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		style.Colors[ImGuiCol_FrameBgActive]        = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
		style.Colors[ImGuiCol_TitleBg]              = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		style.Colors[ImGuiCol_TitleBgActive]        = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		style.Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		style.Colors[ImGuiCol_MenuBarBg]            = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);

		style.Colors[ImGuiCol_CheckMark]          = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
		style.Colors[ImGuiCol_SliderGrab]         = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
		style.Colors[ImGuiCol_SliderGrabActive]   = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		style.Colors[ImGuiCol_Button]             = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		style.Colors[ImGuiCol_ButtonHovered]      = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
		style.Colors[ImGuiCol_ButtonActive]       = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
		style.Colors[ImGuiCol_Header]             = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
		style.Colors[ImGuiCol_HeaderHovered]      = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
		style.Colors[ImGuiCol_HeaderActive]       = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		style.Colors[ImGuiCol_Separator]          = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
		style.Colors[ImGuiCol_SeparatorHovered]   = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
		style.Colors[ImGuiCol_SeparatorActive]    = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
		style.Colors[ImGuiCol_ResizeGrip]         = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
		style.Colors[ImGuiCol_ResizeGripHovered]  = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		style.Colors[ImGuiCol_ResizeGripActive]   = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);
		style.Colors[ImGuiCol_Tab]                = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
		style.Colors[ImGuiCol_TabHovered]         = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
		style.Colors[ImGuiCol_TabActive]          = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
		style.Colors[ImGuiCol_TabUnfocused]       = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
		style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
		style.Colors[ImGuiCol_DockingPreview]     = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
		style.Colors[ImGuiCol_DockingEmptyBg]     = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
		style.FramePadding                        = ImVec2(8.0f, 4.0f);
		style.ItemSpacing                         = ImVec2(8.0f, 4.0f);
		style.IndentSpacing                       = 20.0f;
		style.ScrollbarSize                       = 16.0f;
	}

	static void SetThemeDracula()
	{
		auto& style = ImGui::GetStyle();

		style.Colors[ImGuiCol_WindowBg]             = ImVec4{ 0.1f, 0.1f, 0.13f, 1.0f    };
		style.Colors[ImGuiCol_MenuBarBg]            = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f  };
		style.Colors[ImGuiCol_Border]               = ImVec4{ 0.44f, 0.37f, 0.61f, 0.29f };
		style.Colors[ImGuiCol_BorderShadow]         = ImVec4{ 0.0f, 0.0f, 0.0f, 0.24f    };
		style.Colors[ImGuiCol_Text]                 = ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f     };
		style.Colors[ImGuiCol_TextDisabled]         = ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f     };
		style.Colors[ImGuiCol_Header]               = ImVec4{ 0.13f, 0.13f, 0.17f, 1.0f  };
		style.Colors[ImGuiCol_HeaderHovered]        = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f   };
		style.Colors[ImGuiCol_HeaderActive]         = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f  };
		style.Colors[ImGuiCol_Button]               = ImVec4{ 0.13f, 0.13f, 0.17f, 1.0f  };
		style.Colors[ImGuiCol_ButtonHovered]        = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f   };
		style.Colors[ImGuiCol_ButtonActive]         = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f  };
		style.Colors[ImGuiCol_CheckMark]            = ImVec4{ 0.74f, 0.58f, 0.98f, 1.0f  };
		style.Colors[ImGuiCol_PopupBg]              = ImVec4{ 0.1f, 0.1f, 0.13f, 0.92f   };
		style.Colors[ImGuiCol_SliderGrab]           = ImVec4{ 0.44f, 0.37f, 0.61f, 0.54f };
		style.Colors[ImGuiCol_SliderGrabActive]     = ImVec4{ 0.74f, 0.58f, 0.98f, 0.54f };
		style.Colors[ImGuiCol_FrameBg]              = ImVec4{ 0.13f, 0.13f, 0.17f, 1.0f  };
		style.Colors[ImGuiCol_FrameBgHovered]       = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f   };
		style.Colors[ImGuiCol_FrameBgActive]        = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f  };
		style.Colors[ImGuiCol_Tab]                  = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f  };
		style.Colors[ImGuiCol_TabHovered]           = ImVec4{ 0.24f, 0.24f, 0.32f, 1.0f  };
		style.Colors[ImGuiCol_TabActive]            = ImVec4{ 0.2f, 0.22f, 0.27f, 1.0f   };
		style.Colors[ImGuiCol_TabUnfocused]         = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f  };
		style.Colors[ImGuiCol_TabUnfocusedActive]   = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f  };
		style.Colors[ImGuiCol_TitleBg]              = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f  };
		style.Colors[ImGuiCol_TitleBgActive]        = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f  };
		style.Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f  };
		style.Colors[ImGuiCol_ScrollbarBg]          = ImVec4{ 0.1f, 0.1f, 0.13f, 1.0f    };
		style.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f  };
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f   };
		style.Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4{ 0.24f, 0.24f, 0.32f, 1.0f  };
		style.Colors[ImGuiCol_Separator]            = ImVec4{ 0.44f, 0.37f, 0.61f, 1.0f  };
		style.Colors[ImGuiCol_SeparatorHovered]     = ImVec4{ 0.74f, 0.58f, 0.98f, 1.0f  };
		style.Colors[ImGuiCol_SeparatorActive]      = ImVec4{ 0.84f, 0.58f, 1.0f, 1.0f   };
		style.Colors[ImGuiCol_ResizeGrip]           = ImVec4{ 0.44f, 0.37f, 0.61f, 0.29f };
		style.Colors[ImGuiCol_ResizeGripHovered]    = ImVec4{ 0.74f, 0.58f, 0.98f, 0.29f };
		style.Colors[ImGuiCol_ResizeGripActive]     = ImVec4{ 0.84f, 0.58f, 1.0f, 0.29f  };
		style.Colors[ImGuiCol_DockingPreview]       = ImVec4{ 0.44f, 0.37f, 0.61f, 1.0f  };

		style.TabRounding       = 4;
		style.ScrollbarRounding = 9;
		style.WindowRounding    = 7;
		style.GrabRounding      = 3;
		style.FrameRounding     = 3;
		style.PopupRounding     = 4;
		style.ChildRounding     = 4;
	}

	void Set(Style theme)
	{
		switch (theme)
		{
		using enum Style;

		case DefaultDark    : ImGui::StyleColorsDark();    break;
		case DefaultLight   : ImGui::StyleColorsLight();   break;
		case Classic        : ImGui::StyleColorsClassic(); break;
		case SimpleDark     : SetThemeSimpleDark();        break;
		case DeepDark       : SetThemeDeepDark();          break;
		case CatpuccinMocha : SetThemeCatpuccinMocha();    break;
		case FluentDark     : SetThemeFluentDark();        break;
		case Dracula        : SetThemeDracula();           break;
		default             : ImGui::StyleColorsDark();    break;
		}
	}
}
