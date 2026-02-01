#pragma once
#include "Editor/Panels/ComponentPanels/ComponentPanel.h"
#include "Game/Components/TransformComponent.h"
#include <ImGui/imgui.h>

namespace Ryu::Editor
{
	class TransformComponentPanel : public ComponentPanel<Game::Transform>
	{
	public:
		void DrawComponentUI(Game::Transform& component, Game::Entity)
		{
			ImGui::DragFloat3("Position##TransformComponentPanel", &component.Position.x, 0.1f, ImGuiSliderFlags_ColorMarkers);
			//ImGui::DragFloat3("Rotation##TransformComponentPanel", &component.rotation, 0.1f, ImGuiSliderFlags_ColorMarkers);
			ImGui::DragFloat3("Scale##TransformComponentPanel", &component.Scale.x, 0.1f, 0.01f, ImGuiSliderFlags_ColorMarkers);
		}
	};
}
