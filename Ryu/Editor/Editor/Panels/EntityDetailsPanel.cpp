#include "Editor/Panels/EntityDetailsPanel.h"
#include "Game/Components/EntityMetadata.h"
#include "Game/Components/TransformComponent.h"
#include <ImGui/imgui.h>

namespace Ryu::Editor
{
	class TransformComponentPanel : public ComponentPanel<Game::Transform>
	{
	public:
		void DrawComponentUI(Game::Transform& component, Game::Entity )
		{
			ImGui::DragFloat3("Position", &component.Position.x, 0.1f, ImGuiSliderFlags_ColorMarkers);
			//ImGui::DragFloat3("Rotation", &component.rotation, 0.1f, ImGuiSliderFlags_ColorMarkers);
			ImGui::DragFloat3("Scale", &component.Scale.x, 0.1f, 0.01f, ImGuiSliderFlags_ColorMarkers);
		}
	};

	EntityDetailsPanel::EntityDetailsPanel(EditorApp* app, OutlinerPanel* outlinerPanel)
		: IEditorPanel(app)
		, m_outlinerPanel(outlinerPanel)
	{
		RegisterComponentPanel<Game::Transform>(std::make_unique<TransformComponentPanel>());
	}
	
	EntityDetailsPanel::~EntityDetailsPanel() = default;
	
	void EntityDetailsPanel::OnImGuiRender()
	{
		if (ImGui::Begin("Entity Details##EntityDetailsPanel", &m_isOpen))
		{
			if (m_selectedEntity = m_outlinerPanel->GetSelectedEntity(); m_selectedEntity.IsValid())
			{
				DrawAddComponentButton();
				ImGui::Separator();

				DrawEntityMetadata();
				ImGui::Separator();

				DrawComponents();
				ImGui::Spacing();
				ImGui::Separator();
			}
		}
		ImGui::End();
	}

	void EntityDetailsPanel::RegisterComponentType(const std::string& componentName, ComponentFactory factory)
	{
		m_availableComponents[componentName] = std::move(factory);
	}
	
	std::vector<IComponentPanel*> EntityDetailsPanel::GetComponentPanelsForSelectedEntity()
	{
		std::vector<IComponentPanel*> panels;
		if (!m_selectedEntity.IsValid())
		{
			return panels;
		}

		// Collect all registered panels that have components on this entity
		for (auto& [typeHash, panel] : m_componentPanels)
		{
			if (panel && panel->CanHandle(typeHash))
			{
				panels.push_back(panel.get());
			}
		}

		return panels;
	}

	void EntityDetailsPanel::DrawComponents()
	{
		if (!m_selectedEntity.IsValid())
		{
			return;
		}

		auto panels = GetComponentPanelsForSelectedEntity();
		if (panels.empty())
		{
			ImGui::TextDisabled("No registered component panels");
			return;
		}

		for (IComponentPanel* panel : panels)
		{
			DrawComponentPanel(panel);
		}
	}

	void EntityDetailsPanel::DrawEntityMetadata()
	{
		if (!m_selectedEntity.IsValid())
		{
			return;
		}

		const auto& metadata = m_selectedEntity.GetComponent<Game::EntityMetadata>();
		
		ImGui::Text("Entity: %s", metadata.Name.c_str());
		ImGui::TextDisabled("UUID: %s", metadata.GetUUIDPretty().c_str());
	}
	
	void EntityDetailsPanel::DrawComponentPanel(IComponentPanel* panel)
	{
		if (!m_selectedEntity.IsValid())
		{
			return;
		}

		const char* componentName = panel->GetComponentName();

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed;

		bool nodeOpen = ImGui::CollapsingHeader(componentName, flags);
		if (nodeOpen)
		{
			ImGui::Indent();
			panel->OnImGuiRender(m_selectedEntity);
			ImGui::Unindent();
		}
	}
	
	void EntityDetailsPanel::DrawAddComponentButton()
	{
		if (!m_selectedEntity.IsValid())
		{
			return;
		}

		if (ImGui::Button("Add Component##EntityDetailsPanel", ImVec2(-1, 0)))
		{
			ImGui::OpenPopup("AddComponentPopup##EntityDetailsPanel");
		}

		if (ImGui::BeginPopup("AddComponentPopup##EntityDetailsPanel"))
		{
			for (auto& [componentName, factory] : m_availableComponents)
			{
				if (ImGui::MenuItem(componentName.c_str()))
				{
					factory(m_selectedEntity);
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}
	}
}
