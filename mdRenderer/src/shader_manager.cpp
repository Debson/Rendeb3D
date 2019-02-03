#include "shader_manager.h"


namespace md
{
namespace engine
{
	namespace shaders
	{
		graphics::Shader defaultShader;
		graphics::Shader modelShader;
	}

	void shaders::Init()
	{
		defaultShader	= mdGraphics::Shader("shaders//default.vert", "shaders//default.frag");
		modelShader = mdGraphics::Shader("shaders//default.vert", "shaders//default.frag");
	}

	graphics::Shader *shaders::Default()
	{
		return &defaultShader;
	}

	graphics::Shader *shaders::Model()
	{
		return &modelShader;
	}
}
}