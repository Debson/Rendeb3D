#include "gui.h"


#include <glad/glad.h>


namespace md
{
	namespace engine
	{

	}

	bool engine::Gui::m_RenderGui = true;

	engine::Gui::~Gui() { }

	void engine::Gui::RenderGUI() { }

	void engine::Gui::RenderFPS()
	{
		if (m_RenderGui)
		{
			ImGui::Begin("_DEBUG_");
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}
	}

	void engine::Gui::SetRenderGUI(b8 val)
	{
		m_RenderGui = val;
	}
}