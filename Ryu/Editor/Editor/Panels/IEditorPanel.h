#pragma once

namespace Ryu::Utils { class FrameTimer; }

namespace Ryu::Editor
{
	class EditorApp;

	class IEditorPanel
	{
	public:
		IEditorPanel(EditorApp* app) : m_editorApp(app) {}

		virtual void OnImGuiRender() = 0;
		virtual void OnTick(const Utils::FrameTimer&) {}

	private:
		EditorApp* m_editorApp = nullptr;
	};
}
