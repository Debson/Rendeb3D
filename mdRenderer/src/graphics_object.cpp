#include "objects.h"

#include "md_load_texture.h"
#include "time.h"
#include "component.h"

namespace md
{
	namespace engine
	{
	}


	engine::Graphics::Graphics(graphics::Type type) : Object("lol")
	{
		m_Renderable.m_Type = type;
		m_Renderable.InitializeFromVertices();

		switch (m_Renderable.m_Type)
		{
		case (graphics::Type::tCube): {
			//InitializeFromVertices(vertices::cubeVertices);
			break;
		}
		case (graphics::Type::tPlane): {

			break;
		}
		}
	}

	engine::Graphics::Graphics(std::string const &name, std::string const &path) : Object(name)
	{
		m_Renderable.m_Type = graphics::Type::tModel;
		m_ModelController = std::make_unique<graphics::ModelController>(path);
	}

	engine::Graphics::~Graphics() 
	{ 
		//delete m_ModelController;
	}

	void engine::Graphics::Render()
	{

	}

	void engine::Graphics::Render(mdGraphics::Shader *shader)
	{
		shader->use();
		shader->UpdateMatrices();

		
		if (m_Renderable.m_Type == graphics::Type::tModel)
		{
			if (m_ModelController->IsAnimated())
			{
				f64 runningTime = time::Time();
				//std::cout << runningTime << std::endl;

				std::vector<glm::mat4> transforms;
				m_ModelController->AnimateBones(runningTime, transforms);

				for (u32 i = 0; i < transforms.size(); i++)
				{
					shader->setMat4("gBones[" + std::to_string(i) + "]", transforms[i]);
				}
			}
			m_ModelController->DrawModel(shader);
		}
		else
			m_Renderable.Draw();
	}

	void engine::Graphics::ApplyTexture(GLuint tex)
	{
		m_Renderable.m_Texture = tex;
	}

	engine::graphics::ModelController *engine::Graphics::GetModelController()
	{
		return m_ModelController.get();
	}

	void engine::Graphics::ApplyTexture(std::string texPath)
	{
		m_Renderable.m_Texture = mdLoadTexture(texPath);
	}

}