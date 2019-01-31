#include "physics.h"

#include <gtc/matrix_transform.hpp>

namespace md
{
	mdPhysics::Transform::Transform() : position(0.f), localScale(1.f), m_RotationAngle(0.f), m_Rotated(false) { }

	//mdPhysics::Transform::~Transform() { }

	void mdPhysics::Transform::Rotate(f32 xAngle, f32 yAngle, f32 zAngle)
	{
		if (xAngle != 0)
		{
			Rotate(glm::vec3(1.f, 0.f, 0.f), xAngle);
		}
		if (yAngle != 0)
		{
			Rotate(glm::vec3(0.f, 1.f, 0.f), yAngle);
		}
		if (zAngle != 0)
		{
			Rotate(glm::vec3(0.f, 0.f, 1.f), zAngle);
		}
	}

	void mdPhysics::Transform::Rotate(glm::vec3 axis, f32 angle)
	{
		if (angle != 0)
		{
			m_Rotated = true;
			m_RotateModel = glm::mat4(1.f);
			m_RotateModel = glm::translate(m_RotateModel, position);
			m_RotateModel = glm::scale(m_RotateModel, localScale);

			m_RotationAngle += axis * angle;
			if (m_RotationAngle.x != 0)
				m_RotateModel = glm::rotate(m_RotateModel, glm::radians(m_RotationAngle.x), glm::vec3(1.f, 0.f, 0.f));
			if (m_RotationAngle.y != 0)
				m_RotateModel = glm::rotate(m_RotateModel, glm::radians(m_RotationAngle.y), glm::vec3(0.f, 1.f, 0.f));
			if (m_RotationAngle.z != 0)
				m_RotateModel = glm::rotate(m_RotateModel, glm::radians(m_RotationAngle.z), glm::vec3(0.f, 0.f, 1.f));
		}
	}


	bool mdPhysics::Transform::hasChanged()
	{
		return m_PrevPos != position || m_PrevScale != localScale;
	}

	void mdPhysics::Transform::updateMatrices(glm::mat4 &model)
	{
		if (m_Rotated)
			model = m_RotateModel;
		//else
		{
			model = glm::translate(model, position);
			model = glm::scale(model, localScale);
		}
	}
}