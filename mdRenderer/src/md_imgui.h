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
		struct transition_t;
	}
}
}
namespace ImGui 
{
	void AnimationController(md::engine::graphics::Animations *animVec, md::engine::graphics::transition_t *currTrans);
}
#endif // IMGUIPROGRESSBAR_H