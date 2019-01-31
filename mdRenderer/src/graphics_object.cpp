#include "objects.h"

#include "md_load_texture.h"
#include "time.h"

namespace md
{
	engine::Graphics::Graphics() { }

	engine::Graphics::Graphics(graphics::Type type)
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

	engine::Graphics::Graphics(std::string name, std::string path)
	{
		m_Renderable.m_Type = graphics::Type::tModel;
		m_Model = new graphics::Model(path);
	}

	engine::Graphics::~Graphics() 
	{ 
		delete m_Model;
	}

	void engine::Graphics::Render(mdGraphics::Shader *shader)
	{
		shader->use();
		shader->UpdateMatrices();

		
		if (m_Renderable.m_Type == graphics::Type::tModel)
		{
			if (m_Model->m_hasBones)
			{
				f64 runningTime = time::Time();
				//std::cout << runningTime << std::endl;

				std::vector<glm::mat4> transforms;
				this->m_Model->BoneTransform(runningTime, transforms);

				for (u32 i = 0; i < transforms.size(); i++)
				{
					shader->setMat4("gBones[" + std::to_string(i) + "]", transforms[i]);
				}
			}
			m_Model->DrawModel(shader);
		}
		else
			m_Renderable.Draw();
	}

	void engine::Graphics::ApplyTexture(GLuint tex)
	{
		m_Renderable.m_Texture = tex;
	}

	engine::graphics::Model *engine::Graphics::GetModel()
	{
		return m_Model;
	}

	void engine::Graphics::ApplyTexture(std::string texPath)
	{
		m_Renderable.m_Texture = mdLoadTexture(texPath);
	}
}