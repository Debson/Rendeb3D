#include "objects.h"

namespace md
{
	namespace engine
	{
		f32 scale = 1.f;
	}

	engine::GameObject::GameObject() { }

	engine::GameObject::GameObject(std::string name, graphics::Type type) : Object(name)
	{ 
		graphics = new Graphics(type);
	}

	engine::GameObject::GameObject(std::string name, std::string path) : Object(name) 
	{ 
		graphics = new Graphics(name, path);
	}

	engine::GameObject::~GameObject() 
	{ 
		delete graphics;
	}

	void engine::GameObject::Render(mdGraphics::Shader *shader)
	{
		shader->use();
		transform.matrixModel = glm::mat4(1.f);
		transform.updateMatrices(transform.matrixModel);
		shader->UpdateMatrices();
		shader->setMat4("model", transform.matrixModel);

		graphics->Render(shader);

		RenderGUI();
	}

	void engine::GameObject::RenderGUI()
	{
		if (m_RenderGui)
		{
			ImGui::Begin("_DEBUG_");
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
			ImGui::End();
		}
	}
}