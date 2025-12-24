#include "Testbench/TestbenchWorld.h"
#include "Core/Logging/Logger.h"
#include <External/ImGui/imgui.h>

void TestbenchWorld::OnCreate()
{
	RYU_LOG_DEBUG("Testbench World Created");
}

void TestbenchWorld::OnDestroy()
{
	RYU_LOG_DEBUG("Testbench World Destroyed");
}

void TestbenchWorld::OnTick(const Ryu::Utils::FrameTimer& timer)
{
	m_timer = timer;
}

#if defined(RYU_WITH_EDITOR)
void TestbenchWorld::OnImGuiRender()
{
	// Draw debug info overlay
	{
		static int location = 1;
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration
			| ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_AlwaysAutoResize
			| ImGuiWindowFlags_NoSavedSettings
			| ImGuiWindowFlags_NoFocusOnAppearing
			| ImGuiWindowFlags_NoNav;

		if (location >= 0)
		{
			ImVec2 windowPos, windowPosPivot;

			constexpr f32 PAD = 10.0f;
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImVec2 workPos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
			ImVec2 workSize = viewport->WorkSize;

			windowPos.x      = (location & 1) ? (workPos.x + workSize.x - PAD) : (workPos.x + PAD);
			windowPos.y      = (location & 2) ? (workPos.y + workSize.y - PAD) : (workPos.y + PAD);
			windowPosPivot.x = (location & 1) ? 1.0f : 0.0f;
			windowPosPivot.y = (location & 2) ? 1.0f : 0.0f;

			ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPosPivot);
			ImGui::SetNextWindowViewport(viewport->ID);
			windowFlags |= ImGuiWindowFlags_NoMove;
		}
		else if (location == -2)
		{
			// Center window
			ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
			windowFlags |= ImGuiWindowFlags_NoMove;
		}

		ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background

		if (ImGui::Begin("Info", nullptr, windowFlags))
		{
			ImGui::Text("World Info");
			ImGui::Separator();
			ImGui::Text("Delta Time: %.3fms", m_timer.DeltaTimeF() * 1000.0f);
			ImGui::Text("FPS: %.2f", m_timer.FPS());
			ImGui::Text("Total Time: %.2f", m_timer.TimeSinceStart<std::chrono::seconds>());
			ImGui::Text("Frame Count: %llu", m_timer.FrameCount());

			if (ImGui::BeginPopupContextWindow())
			{
				if (ImGui::MenuItem("Custom", NULL, location == -1)) location = -1;
				if (ImGui::MenuItem("Center", NULL, location == -2)) location = -2;
				if (ImGui::MenuItem("Top-left", NULL, location == 0)) location = 0;
				if (ImGui::MenuItem("Top-right", NULL, location == 1)) location = 1;
				if (ImGui::MenuItem("Bottom-left", NULL, location == 2)) location = 2;
				if (ImGui::MenuItem("Bottom-right", NULL, location == 3)) location = 3;

				ImGui::EndPopup();
			}
		}
		ImGui::End();
	}

}
#endif
