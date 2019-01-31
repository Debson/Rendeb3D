#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "md_shader.h"

#include <vector>

#include <glad/glad.h>

#include "camera.h"

namespace md
{
	namespace engine
	{
		namespace graphics
		{
			enum Type
			{
				tModel,
				tCube,
				tPlane
			};

			class Shader : public mdShader
			{
			public:
				Shader();
				Shader(const GLchar *vertexPath, const GLchar *fragmentPath, const GLchar *geometryPath = nullptr);
				virtual ~Shader();

				void Configure(Camera &cam);
				void UpdateMatrices();

			private:

			};

			class Renderer
			{
			public:

				static void SetCamera(Camera *cam);
				static Camera *GetCamera();
					
			private:
				static Camera *m_Camera;
			};


			class Renderable
			{
			public:

				Renderable();
				//Renderable(std::string);
				//virtual ~Renderable();

				void Draw();
				void InitializeFromVertices();

				Type m_Type;
				GLuint m_Texture;
			protected:

			private:
				void InitializeCube();

				GLuint m_VAO, m_VBO, m_EBO;
				GLuint m_VertSize;
			
			};
		}
	}

	namespace mdGraphics = engine::graphics;
}

#endif // !GRAPHICS_H
