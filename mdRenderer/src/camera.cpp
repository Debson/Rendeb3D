#include "camera.h"


namespace md
{
	engine::Camera::Camera(glm::vec2 screenRes, glm::vec3 Pos, glm::vec3 Up, float Yaw, float Pitch) :
		m_ScreenRes(screenRes), 
		front(glm::vec3(0.f, 0.f, -1.f)), 
		movementSpeed(SPEED), 
		mouseSensitivity(SENSITIVITY), 
		zoom(ZOOM), Object("Camera")
	{
		position = Pos;
		worldUp = Up;
		yaw = Yaw;
		pitch = Pitch;
		updateCameraVectors();
	}

	engine::Camera::Camera(glm::vec3 Pos, glm::vec3 Up, float Yaw, float Pitch) :
		front(glm::vec3(0.f, 0.f, -1.f)), 
		movementSpeed(SPEED), 
		mouseSensitivity(SENSITIVITY), 
		zoom(ZOOM),
		Object("Camera")
	{
		position = Pos;
		worldUp = Up;
		yaw = Yaw;
		pitch = Pitch;
		updateCameraVectors();
	}

	engine::Camera::Camera(f32 posX, f32 posY, f32 posZ, f32 upX, f32 upY, f32 upZ, f32 Yaw, f32 Pitch) :
		front(glm::vec3(0.f, 0.f, -1.f)), 
		movementSpeed(SPEED), 
		mouseSensitivity(SENSITIVITY), 
		zoom(ZOOM),
		Object("Camera")
	{
		position = glm::vec3(posX, posY, posZ);
		worldUp = glm::vec3(upX, upY, upZ);
		yaw = Yaw;
		pitch = Pitch;
		updateCameraVectors();
	}

	glm::mat4 &engine::Camera::GetViewMatrix()
	{
		m_View = glm::lookAt(position, position + front, up);
		return m_View;
	}

	glm::mat4 &engine::Camera::GetProjectionMatrix()
	{
		m_Projection = glm::perspective(glm::radians(zoom), m_ScreenRes.x / m_ScreenRes.y, 0.1f, 1000.f);
		return m_Projection;
	}

	void engine::Camera::ProcessKeyboard(CameraMovement dir, f64 dT, float speed)
	{
		f32 velocity = speed * dT;

		switch (dir)
		{
		case(CameraMovement::Forward): {
			position += front * velocity;
			break;
		}
		case(CameraMovement::Backward): {
			position -= front * velocity;
			break;
		}
		case(CameraMovement::Left): {
			position -= right * velocity;
			break;
		}
		case(CameraMovement::Right): {
			position += right * velocity;
			break;
		}
		}
	}

	void engine::Camera::ProcessMouseMovement(f32 offsetX, f32 offsetY, bool constrainPitch)
	{
		offsetX *= mouseSensitivity;
		offsetY *= mouseSensitivity;

		yaw += offsetX;
		pitch -= offsetY;

		if (constrainPitch)
		{
			if (pitch > 89.f)
				pitch = 89.f;
			if (pitch < -89.f)
				pitch = -89.f;
		}

		updateCameraVectors();
	}

	void engine::Camera::ProcessMouseScroll(f32 offsetY)
	{
		if (zoom >= 1.f && zoom <= 45.f)
			zoom -= offsetY;
		if (zoom <= 1.f)
			zoom = 1.f;
		if (zoom >= 45.f)
			zoom = 45.f;
	}

	void engine::Camera::RenderGUI()
	{
		if (m_RenderGui)
		{
			ImGui::Begin("_DEBUG_");
			if (ImGui::TreeNode("Camera") == true)
			{
				ImGui::Indent();
				ImGui::Text("Pos x: %f  Pos y: %f  Pos Z: %f", position.x, position.y, position.z);
				ImGui::Text("Yaw: %f  Pitch: %f", yaw, pitch);

				ImGui::TreePop();
			}
			ImGui::End();
		}
	}

	void engine::Camera::updateCameraVectors()
	{
		glm::vec3 Front;
		Front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		Front.y = sin(glm::radians(pitch));
		Front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		front = glm::normalize(Front);

		right = glm::normalize(glm::cross(front, worldUp));
		up = glm::normalize(glm::cross(right, front));
	}
}