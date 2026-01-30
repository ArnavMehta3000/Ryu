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

		inline bool IsOpen() const { return m_isOpen; }
		inline void SetOpen(bool isOpen) { m_isOpen = isOpen; }

		virtual void OnImGuiRender() = 0;

	protected:
		bool m_isOpen = true;

	private:
		EditorApp* m_editorApp = nullptr;
	};
}
