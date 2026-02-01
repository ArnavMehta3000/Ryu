#pragma once
#include "Editor/Panels/IEditorPanel.h"

namespace Ryu::Editor
{
	class MenuBarPanel : public IEditorPanel
	{
	public:
		static constexpr auto Name = "Menu Bar";

	public:
		using IEditorPanel::IEditorPanel;
		virtual ~MenuBarPanel() override = default;

		void OnImGuiRender() override;

	private:
		void CachePanels();

	private:
		class OutlinerPanel* m_outliner = nullptr;
		class EntityDetailsPanel* m_entityDetails = nullptr;
		class AssetsPanel* m_assetsPanel = nullptr;
	};
}
