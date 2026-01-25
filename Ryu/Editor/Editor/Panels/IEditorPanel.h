#pragma once

namespace Ryu::Utils { class FrameTimer; }

namespace Ryu::Editor
{
	class EditorApp;

	class IEditorPanel
	{
	public:
		IEditorPanel(EditorApp* app) : m_editorApp(app) {}
		virtual ~IEditorPanel() {}

		virtual void OnImGuiRender() = 0;

	private:
		EditorApp* m_editorApp = nullptr;
	};
}
