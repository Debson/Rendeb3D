#include "objects.h"
#include "physics.h"
#include "shader_manager.h"

namespace md
{
	namespace engine
	{
		std::vector< std::unique_ptr< GameObject > > mdGameObjectsContainer;
		f32 scale = 1.f;
	}

	u32 engine::GameObject::numberOfGameObjects = 0;

	/*engine::GameObject::GameObject() 
	{
		mdGameObjectsContainer.emplace_back(std::make_unique<GameObject>("GameObject" + std::to_string(numberOfGameObjects)));
		numberOfGameObjects++;
	}*/

	engine::GameObject::GameObject(std::string const &name, graphics::Type type)
	{ 
		m_Name = name;
		graphics = new Graphics(type);
		numberOfGameObjects++;
	}

	engine::GameObject::GameObject(std::string const &name, std::string path)
	{ 
		m_Name = name;
		graphics = new Graphics(name, path);
		m_Shader = shaders::Model();
		numberOfGameObjects++;
	}

	engine::GameObject::~GameObject() 
	{ 
		delete graphics;
	}

	void engine::GameObject::Render()
	{
		if (!m_Shader)
			return;

		m_Shader->use();
		transform.matrixModel = glm::mat4(1.f);
		transform.updateMatrices(transform.matrixModel);
		m_Shader->UpdateMatrices();
		m_Shader->setMat4("model", transform.matrixModel);

		graphics->Render(m_Shader);

		RenderGUI();
	}

	/*engine::GameObject &engine::GameObject::CreatePrimitive(graphics::Type type)
	{

	}*/

	engine::GameObject *engine::GameObject::LoadModel(std::string const& name, std::string const &path)
	{
		//const auto &model = std::make_unique<GameObject>(name, path);
		mdGameObjectsContainer.emplace_back(std::make_unique<GameObject>(name, path));

		return mdGameObjectsContainer.back().get();
	}

	engine::GameObject *engine::GameObject::Create(std::string const &name)
	{
		mdGameObjectsContainer.emplace_back(std::make_unique<GameObject>(name));

		return mdGameObjectsContainer.back().get();
	}

	std::vector< std::unique_ptr< engine::GameObject > > &engine::GameObject::GetGameObjectsContainer()
	{
		return mdGameObjectsContainer;
	}

	engine::GameObject *engine::GameObject::Find(std::string const &name)
	{
		for (auto & gameObject : mdGameObjectsContainer)
		{
			if (gameObject->m_Name == name)
				return gameObject.get();
		}

		assert(0);

		return (GameObject*)nullptr;
	}

	void engine::GameObject::RenderGUI()
	{
		if (m_RenderGui)
		{
			/*ImGui::Begin("_DEBUG_");
			if (ImGui::TreeNode(m_Name.c_str()))
			{
				ImGui::SliderFloat("Position X", &transform.position.x, -50.1f, 50.1f);
				ImGui::SliderFloat("Position Y", &transform.position.y, -50.1f, 50.1f);
				ImGui::SliderFloat("Position Z", &transform.position.z, -50.1f, 50.1f);

				if(ImGui::SliderFloat("Scale", &scale,	0.f, 10.f))
					transform.localScale = glm::vec3(scale);
				ImGui::SliderFloat("Scale X", &transform.localScale.x, 0.f, 10.f);
				ImGui::SliderFloat("Scale Y", &transform.localScale.y, 0.f, 10.f);
				ImGui::SliderFloat("Scale Z", &transform.localScale.z, 0.f, 10.f);
				ImGui::Unindent();
				ImGui::TreePop();
			}
			ImGui::End();*/
		}
	}

	

}