#include "gui.h"


#include <GL/gl3w.h>


namespace md
{
	namespace gui
	{

	}

	bool gui::Gui::m_RenderGui = true;

	gui::Gui::~Gui() { }

	void gui::Gui::RenderGUI() { }

	void gui::Gui::RenderFPS()
	{
		if (m_RenderGui)
		{
			ImGui::Begin("_DEBUG_");
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}
	}

	void gui::Gui::SetRenderGUI(b8 val)
	{
		m_RenderGui = val;
	}
}