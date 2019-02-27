#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <iostream>

//#include "component.h"
#include "objects.h"
#include "camera.h"

namespace md
{
	namespace environment
	{

		class Skybox : public engine::Object
		{
			CLASS_DECLARATION(Skybox);

		public:
			Skybox(std::vector<std::string> const &faces);

			static void Render();
			static b8 IsEnabled();

			static GLuint textureID;
		private:
			static GLuint m_VAO, m_VBO;
			static engine::Camera m_Camera;
			static glm::mat4 m_View;
			static b8 m_Enabled;
		};

	}
}

#endif // !ENVIRONMENT_H
