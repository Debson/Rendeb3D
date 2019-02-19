#ifndef IMGUIPROGRESSBAR_H
#define  IMGUIPROGRESSBAR_H

#include <map>
#include <iostream>

#include "../external/imgui/imgui.h"

namespace md
{
namespace engine
{
	namespace graphics
	{
		struct anim_t;
		typedef std::map<std::string, anim_t*> Animations;
	}
}
}
namespace ImGui {

	// Please note that you can tweak the "format" argument if you want to add a prefix (or a suffix) piece of text to the text that appears at the right of the bar.
	// returns the value "fraction" in 0.f-1.f.
	// It does not need any ID.
	float ProgressBar(const char* optionalPrefixText, float value, const float minValue = 0.f, const float maxValue = 1.f, const char* format = "%1.0f%%", const ImVec2& sizeOfBarWithoutTextInPixels = ImVec2(-1, -1),
		const ImVec4& colorLeft = ImVec4(0, 1, 0, 0.8), const ImVec4& colorRight = ImVec4(0, 0.4, 0, 0.8), const ImVec4& colorBorder = ImVec4(0.25, 0.25, 1.0, 1));
	

	void AnimationController(md::engine::graphics::Animations const *animVec, const char* format = "%1.0f%%", const ImVec2& sizeOfBarWithoutTextInPixels = ImVec2(-1, -1),
		const ImVec4& colorLeft = ImVec4(0.0, 0.0, 0.0, 0.5), const ImVec4& colorRight = ImVec4(0.0, 0.0, 0.0, 0.5), const ImVec4& colorBorder = ImVec4(1.0, 0.0, 0.0, 0.8));


	void ShowExampleAppCustomNodeGraph(md::engine::graphics::Animations *animVec);

	void TestProgressBar();
}
#endif // IMGUIPROGRESSBAR_H