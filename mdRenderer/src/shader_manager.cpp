#include "shader_manager.h"


namespace md
{
namespace engine
{
	namespace shaders
	{
		graphics::Shader defaultShader;
		graphics::Shader modelShader;
		graphics::Shader skyboxShader;
	}

	void shaders::Init()
	{
		defaultShader	= mdGraphics::Shader("shaders//default.vert", "shaders//default.frag");
		modelShader		= mdGraphics::Shader("shaders//model_shader.vert", "shaders//default.frag");
		skyboxShader	= mdGraphics::Shader("shaders//skybox.vert", "shaders//skybox.frag");
	}

	graphics::Shader *shaders::Default()
	{
		return &defaultShader;
	}

	graphics::Shader *shaders::Model()
	{
		return &modelShader;
	}

	graphics::Shader *shaders::Skybox()
	{
		return &skyboxShader;
	}
}
}