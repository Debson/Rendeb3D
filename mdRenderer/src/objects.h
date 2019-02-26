#ifndef OBJECT_H
#define OBJECT_H

#include <iostream>

#include <glad/glad.h>

#include "types.h"
#include "graphics.h"
#include "physics.h"
#include "gui.h"
#include "model_controller.h"
#include "component.h"

namespace md
{
	namespace engine
	{
		class Object : public Component
		{
		public:
			Object(std::string const &name);
			virtual ~Object() = default;

			u32 GetInstanceID();
			std::string ToString();

			static void Destroy(Object *obj, f32 time = 0.f);
			static Object *FindObjectOfName(std::string type);

			bool operator==(const Object & rhs) const;

		protected:
			std::string m_Name;
			u32 m_InstanceID;
			static u32 ObjectCounter;
			void OnCreate(std::string const &name);
		};

		class Graphics : public Object
		{
			CLASS_DECLARATION(Graphics);

		public:
			Graphics(graphics::Type type);
			Graphics(std::string const &name, std::string const &path);
			~Graphics();

			virtual void Render(mdGraphics::Shader *shader);
			virtual void Render();
			virtual void ApplyTexture(GLuint);
			virtual void ApplyTexture(std::string texPath);
			graphics::ModelController *GetModelController();


		private:
				graphics::Renderable		m_Renderable;
				std::unique_ptr< graphics::ModelController > m_ModelController;
				std::string					m_ModelPath;
		};

		class GameObject : gui::Gui
		{
		public:
			GameObject();
			GameObject(std::string const &name, graphics::Type type = graphics::Type::tModel);
			GameObject(std::string const &name, std::string path);
			~GameObject();

			void Render();

			//static GameObject &CreatePrimitive(graphics::Type type);
			static GameObject *LoadModel(std::string const& name, std::string const &path);
			static std::vector< std::unique_ptr< engine::GameObject > > &GetGameObjectsContainer();

			template< class ComponentType, typename... Args >
			void                                    AddComponent(Args&&... params);

			template< class ComponentType >
			ComponentType &                         GetComponent();

			/*template< class ComponentType >
			ComponentType &                         GetComponent();

			template< class ComponentType >
			bool                                    RemoveComponent();

			template< class ComponentType >
			std::vector< ComponentType * >          GetComponents();

			template< class ComponentType >
			int                                     RemoveComponents();*/


			physics::Transform transform;
			std::vector< std::unique_ptr< Component > > components;
			Graphics *graphics;

		protected:
			void RenderGUI();

		private:
			graphics::Shader *m_Shader;
		};

		template< class ComponentType, typename... Args >
		void GameObject::AddComponent(Args&&... params)
		{
			components.emplace_back(std::make_unique< ComponentType >(std::forward< Args >(params)...));
		}

		template< class ComponentType >
		ComponentType & GameObject::GetComponent() {
			for (auto && component : components) {
				if (component->IsClassType(ComponentType::Type))
					return *static_cast<ComponentType *>(component.get());
			}

			return *std::unique_ptr< ComponentType >(nullptr);
		}
	}
}


#endif // !OBJECT_H
