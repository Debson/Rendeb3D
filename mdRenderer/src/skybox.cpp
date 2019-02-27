#include "environment.h"

#include "../external/stb_image.h"
#include "types.h"
#include "debug.h"
#include "shader_manager.h"
#include "vertices.h"
#include "graphics.h"

namespace md
{
	b8 environment::Skybox::m_Enabled = false;
	GLuint environment::Skybox::m_VAO = 0;
	GLuint environment::Skybox::m_VBO = 0;
	engine::Camera *environment::Skybox::m_Camera;
	glm::mat4 environment::Skybox::m_View;
	GLuint environment::Skybox::textureID = 0;

	environment::Skybox::Skybox(std::vector<std::string> const &faces) : Object("Skybox")
	{
		
		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);
		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices::skyboxVertices.size() * sizeof(vertices::skyboxVertices[0]), vertices::skyboxVertices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrChannels;
		for (u32 i = 0; i < faces.size(); i++)
		{
			unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);
			}
			else
			{
				md_error("Cubemap texture failed to load at path: %s", faces[i].c_str());
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		engine::shaders::Skybox()->use();
		engine::shaders::Skybox()->setInt("skybox", 0);

		m_Camera = &engine::GameObject::Find("Camera")->GetComponent<engine::Camera>();

		m_Enabled = true;
	}

	void environment::Skybox::Render()
	{

		glDepthFunc(GL_LEQUAL);
		engine::shaders::Skybox()->use();
		m_View = glm::mat4(glm::mat3(m_Camera->GetViewMatrix()));
		engine::shaders::Skybox()->setMat4("view", m_View);
		engine::shaders::Skybox()->setMat4("projection", m_Camera->GetProjectionMatrix());
		

		glBindVertexArray(m_VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);
		
	}

	b8 environment::Skybox::IsEnabled()
	{
		return m_Enabled;
	}
}