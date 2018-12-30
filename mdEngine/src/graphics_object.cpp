#include "objects.h"

#include "md_load_texture.h"
#include "time.h"

namespace md
{
	engine::Graphics::Graphics() { }

	engine::Graphics::Graphics(graphics::Type type)
	{ 
		m_Type = type;
		InitializeFromVertices();

		switch (m_Type)
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

	engine::Graphics::Graphics(std::string name, std::string path) : Model(path)
	{
		m_Type = graphics::Type::tModel;
	}

	engine::Graphics::~Graphics() { }

	void engine::Graphics::Render(mdGraphics::Shader *shader)
	{
		shader->use();
		shader->UpdateMatrices();

		
		if (m_Type == graphics::Type::tModel)
		{
			if (m_hasBones)
			{
				f64 runningTime = time::Time();
				//std::cout << runningTime << std::endl;

				std::vector<glm::mat4> transforms;
				this->BoneTransform(runningTime, transforms);

				for (u32 i = 0; i < transforms.size(); i++)
				{
					shader->setMat4("gBones[" + std::to_string(i) + "]", transforms[i], true);
				}
			}
			DrawModel(shader);
		}
		else
			Draw();
	}

	void engine::Graphics::ApplyTexture(GLuint tex)
	{
		m_Texture = tex;
	}

	void engine::Graphics::ApplyTexture(std::string texPath)
	{
		m_Texture = mdLoadTexture(texPath);
	}
}