#pragma once
#include "Editor/Panels/ComponentPanels/ComponentPanel.h"
#include "Game/Components/CameraComponent.h"
#include <ImGui/imgui.h>

namespace Ryu::Editor
{
	class CameraComponentPanel : public ComponentPanel<Game::CameraComponent>
	{
	public:
		void DrawComponentUI(Game::CameraComponent& component, Game::Entity)
		{
            using namespace Ryu::Game;
			ImGui::SeparatorText("Projection");

            // Projection mode combo
            {
                constexpr std::array projectionLabels{ "Perspective", "Orthographic" };

                i32 current = (component.Mode == CameraComponent::Projection::Perspective) ? 0 : 1;

                if (ImGui::BeginCombo("Mode####CameraComponentPanel", projectionLabels[static_cast<size_t>(current)]))
                {
                    for (i32 i = 0; i < static_cast<i32>(projectionLabels.size()); ++i)
                    {
                        bool selected = (current == i);
                        if (ImGui::Selectable(projectionLabels[static_cast<size_t>(i)], selected))
                        {
                            current = i;
                            component.Mode = (i == 0)
                                ? CameraComponent::Projection::Perspective
                                : CameraComponent::Projection::Orthographic;
                        }
                        if (selected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
            }

            // Perspective-only settings
            if (component.Mode == CameraComponent::Projection::Perspective)
            {
                ImGui::SliderFloat("Vertical FOV (deg)##CameraComponentPanel", &component.FovY, 1.0f, 179.0f);
            }

            // Orthographic-only settings
            if (component.Mode == CameraComponent::Projection::Orthographic)
            {
                ImGui::DragFloat2("Ortho Size (W,H)##CameraComponentPanel", component.OrthoSize.data(), 0.1f, 0.01f, 10000.0f, "%.3f");
            }

            ImGui::SeparatorText("Clipping##CameraComponentPanel");
            ImGui::DragFloatRange2(
                "Near / Far##CameraComponentPanel",
                component.ClipPlane.data(),
                component.ClipPlane.data() + 1,
                0.01f,
                0.001f,
                100000.0f,
                "Near: %.3f",
                "Far: %.3f"
            );

            // Priority
            {
                i32 prio = static_cast<i32>(component.Priority);
                if (ImGui::DragInt("Priority##CameraComponentPanel", &prio, 1.0f, std::numeric_limits<i32>::min(), std::numeric_limits<i32>::max()))
                {
                    component.Priority = static_cast<u32>(prio);
                }
            }

            if (ImGui::TreeNode("Culling Mask##CameraComponentPanel"))
            {
                // Show 32 flags as labeled bits, grouped by 8
                for (i32 row = 0; row < 4; ++row)
                {
                    for (i32 col = 0; col < 8; ++col)
                    {
                        i32 bitIndex = row * 8 + col;
                        u32 flag = 1u << bitIndex;

                        ImGui::PushID(bitIndex);
                        // CheckboxFlags directly toggles this bit inside cam.CullingMask
                        ImGui::CheckboxFlags(
                            std::format("{}", bitIndex).c_str(),
                            &component.CullingMask,
                            flag
                        );
                        ImGui::SameLine();
                        ImGui::PopID();
                    }
                    ImGui::NewLine();
                }

                ImGui::TreePop();
            }
		}
	};
}
