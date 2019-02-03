#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include "graphics.h"


namespace md
{
namespace engine
{
	namespace shaders
	{
		void Init();
		graphics::Shader *Default();
		graphics::Shader *Model();
	}

}
}

namespace mdShaders = md::engine::shaders;

#endif // !SHADER_MANAGER_H
