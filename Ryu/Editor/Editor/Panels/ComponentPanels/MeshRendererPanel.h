#pragma once
#include "Editor/Panels/ComponentPanels/ComponentPanel.h"

#include "Asset/AssetData.h"
#include "Asset/AssetRegistry.h"
#include "Game/Components/MeshRenderer.h"
#include <ImGui/imgui.h>

namespace Ryu::Editor
{
    class MeshRendererPanel : public ComponentPanel<Game::MeshRenderer>
    {
    protected:
        void DrawComponentUI(Game::MeshRenderer& component, Game::Entity)
        {
            ImGui::Checkbox("Is Visible##MeshRendererPanel", &component.IsVisible);

            const char* meshLabel = component.MeshHandle.IsValid()
                ? "Mesh (drag to replace)"
                : "Mesh (drag to assign)";

            ImGui::Text("%s", meshLabel);
            ImGui::SameLine();


            if (component.MeshHandle.IsValid())
            {
                std::string_view name = component.GetAssetRegistry()->Meshes().GetEntry(component.MeshHandle)->Name;
                ImGui::Text("%s", name.data());
            }
            else
            {
                ImGui::TextDisabled("None");
            }

            // Make the area a drop target
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Mesh"))
                {
                    Asset::AssetId droppedId = *static_cast<const Asset::AssetId*>(payload->Data);
                    component.MeshHandle = Asset::MeshHandle{ droppedId };
                }
                ImGui::EndDragDropTarget();
            }

            // Clear button
            if (component.MeshHandle.IsValid())
            {
                ImGui::SameLine();
                if (ImGui::SmallButton("X##ClearMesh"))
                {
                    component.MeshHandle = {};
                }
            }
        }
    };
}
