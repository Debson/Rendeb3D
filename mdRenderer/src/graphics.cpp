#include "graphics.h"

#include <gtc/matrix_transform.hpp>

#include "vertices.h"
#include "camera.h"

namespace md
{
	mdGraphics::Shader::Shader() { std::cout << "graphics\n"; }

	mdGraphics::Shader::Shader(const GLchar *vertexPath, const GLchar *fragmentPath, const GLchar *geometryPath) : mdShader(vertexPath, fragmentPath, geometryPath)
	{
		this->use();
		this->setMat4("projection", Renderer::GetCamera()->GetProjectionMatrix());
		this->setMat4("view", Renderer::GetCamera()->GetViewMatrix());
	}

	mdGraphics::Shader::~Shader() { }

	void mdGraphics::Shader::Configure(md::engine::Camera &cam)
	{
		/*this->use();
		this->setMat4("projection", cam.GetProjectionMatrix());*/
	}

	void mdGraphics::Shader::UpdateMatrices()
	{
		this->use();
		this->setMat4("projection", Renderer::GetCamera()->GetProjectionMatrix());
		this->setMat4("view", Renderer::GetCamera()->GetViewMatrix());
		
	}

	md::engine::Camera *md::mdGraphics::Renderer::m_Camera = NULL;

	void mdGraphics::Renderer::SetCamera(md::engine::Camera *cam)
	{
		m_Camera = cam;
	}

	md::engine::Camera *mdGraphics::Renderer::GetCamera()
	{
		return m_Camera;
	}

	mdGraphics::Renderable::Renderable() : m_Texture(0) { }

	//mdGraphics::Renderable::Renderable(std::string a) : m_Texture(0) { }

	//mdGraphics::Renderable::~Renderable() { }

	void mdGraphics::Renderable::InitializeFromVertices()
	{
		m_Texture = 0;
		switch (m_Type)
		{
		case (Type::tCube): {
			InitializeCube();
			break;
		}
		case (Type::tPlane): {

			break;
		}
		}
	}

	void mdGraphics::Renderable::Draw()
	{
		if (m_Texture != 0)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_Texture);
		}

		switch (m_Type)
		{
		case (Type::tCube): {
			glBindVertexArray(m_VAO);
			glDrawArrays(GL_TRIANGLES, 0, m_VertSize);
			glBindVertexArray(0);
			break;
		}
		case (Type::tPlane): {

			break;
		}
		}

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void mdGraphics::Renderable::InitializeCube()
	{
		m_VertSize = vertices::cubeVertices.size();

		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_VertSize, vertices::cubeVertices.data(), GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(m_VAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}