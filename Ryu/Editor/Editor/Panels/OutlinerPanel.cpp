#include "Editor/Panels/OutlinerPanel.h"
#include "Game/World/Entity.h"
#include "Game/Components/EntityMetadata.h"
#include <ImGui/imgui.h>
#include <ranges>

namespace Ryu::Editor
{
	OutlinerPanel::OutlinerPanel(EditorApp* app, Game::World* world)
		: IEditorPanel(app)
		, m_world(world)
		, m_selectedEntity(Game::InvalidEntityHandle)
	{
	}
	
	void OutlinerPanel::OnImGuiRender()
	{
		ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Outliner##OutlinerPanel", &m_isOpen, ImGuiWindowFlags_None))
		{
			if (!m_world)  [[unlikely]]
			{
				// No world loaded
				ImGui::Text("No world loaded");
			}
			else
			{
				DrawOutliner(m_world->GetRegistry());
			}
		}
		ImGui::End();
	}

	Game::Entity OutlinerPanel::GetSelectedEntity() const
	{
		return m_world ? m_world->GetEntityFromHandle(m_selectedEntity) : Game::Entity();
	}

	void OutlinerPanel::DrawOutliner(Game::Registry& registry)
	{
		// Get all entities
		auto view = registry.view<Game::EntityMetadata>();		
		ImGui::BeginChild("EntityList##OutlinerPanel");
		{
			for (Game::EntityHandle entity : view)
			{
				if (registry.valid(entity))
				{
					const auto& metadata = registry.get<Game::EntityMetadata>(entity);

					DrawEntityNode(registry, entity, metadata);					
				}
			}
		}
		ImGui::EndChild();

		// Context menu for creating new entities
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows))
		{
			ImGui::OpenPopup("Entity Context Menu##OutlinerPanel");
		}

		if (ImGui::BeginPopup("Entity Context Menu##OutlinerPanel"))
		{
			if (ImGui::MenuItem("Create New Entity"))
			{
				m_world->CreateEntity();  // World will add the Game::EntityMetadata component
			}
			ImGui::EndPopup();
		}
	}
	
	void OutlinerPanel::DrawEntityNode([[maybe_unused]] Game::Registry& registry, Game::EntityHandle entity, const Game::EntityMetadata& metadata)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		// Highlight selected entity
		if (entity == m_selectedEntity)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		const std::string UUID = metadata.GetUUIDPretty();
		const std::string nodeId = metadata.Name + "##" + UUID;

		ImGui::TreeNodeEx(nodeId.c_str(), flags);

		// Handle entity selection
		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		{
			m_selectedEntity = entity;
		}

		// Context menu for entity
		if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
		{
			m_selectedEntity = entity;
			ImGui::OpenPopup("Entity Context Menu##OutlinerPanel");
		}

		if (ImGui::BeginPopup("Entity Context Menu##OutlinerPanel"))
		{
			if (ImGui::MenuItem("Delete"))
			{
				m_world->DestroyEntity(entity);
			}
			ImGui::EndPopup();
		}

		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Entity ID: %s", UUID.c_str());
		}
	}
}
