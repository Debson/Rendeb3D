#ifndef OBJECT_H
#define OBJECT_H

#include <iostream>

#include <GL/gl3w.h>

#include "types.h"
#include "graphics.h"
#include "physics.h"
#include "gui.h"
#include "model.h"



namespace md
{
	namespace engine
	{
		class Object
		{
		public:
			Object();
			Object(std::string name);
			virtual ~Object();

			u32 GetInstanceID();
			std::string ToString();

			static void Destroy(Object *obj, f32 time = 0.f);
			static Object *FindObjectOfName(std::string type);

			bool operator==(const Object & rhs) const;

		protected:
			std::string m_Name;
			u32 m_InstanceID;
			static u32 ObjectCounter;
			void OnCreate(std::string &name);
		};

		class Graphics : public graphics::Renderable, public graphics::Model
		{
		public:
			Graphics();
			explicit Graphics(graphics::Type type);
			Graphics(std::string name, std::string path);
			virtual ~Graphics();

			virtual void Render(mdGraphics::Shader *shader);
			virtual void ApplyTexture(GLuint);
			virtual void ApplyTexture(std::string texPath);


		private:
				std::string m_ModelPath;
		};

		class GameObject : public Object, gui::Gui
		{
		public:
			GameObject();
			GameObject(std::string name, graphics::Type type = graphics::Type::tModel);
			GameObject(std::string name, std::string path);
			~GameObject();

			void Render(mdGraphics::Shader *shader);

			physics::Transform transform;
			Graphics graphics;

		protected:
			void RenderGUI();

		private:
		};

	}
}


#endif // !OBJECT_H
