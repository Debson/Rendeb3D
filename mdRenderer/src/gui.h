#ifndef GUI_H
#define GUI_H

#include "../external/imgui/imgui.h"
#include "../external/imgui/imgui_impl_sdl.h"
#include "../external/imgui/imgui_impl_opengl3.h"

#include "types.h"

namespace md
{
namespace gui
{
	class Gui
	{
	public:
		virtual ~Gui();

		virtual void RenderGUI();
		static void RenderFPS();
		static void SetRenderGUI(b8 val);

	protected:
		static b8 m_RenderGui;
	};
}
}

#endif // !GUI_H
