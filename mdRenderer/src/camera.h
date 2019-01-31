#ifndef CAMERA_H
#define CAMERA_H

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "types.h"
#include "gui.h"


namespace md
{
	namespace engine
	{
		namespace graphics
		{
			enum CameraMovement
			{
				Forward,
				Backward,
				Left,
				Right
			};

			const f32 YAW			= -90.f;
			const f32 PITCH			= 0.f;
			const f32 SPEED			= 5.5f;
			const float SENSITIVITY = 0.1f;
			const float ZOOM		= 45.f;


			class Camera : public gui::Gui
			{
			public:
				Camera(glm::vec2 screenRes, glm::vec3 Pos = glm::vec3(0.f), glm::vec3 Up = glm::vec3(0.f, 1.f, 0.f), float Yaw = YAW, float Pitch = PITCH) :
					m_ScreenRes(screenRes), front(glm::vec3(0.f, 0.f, -1.f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM) 
				{ 
					position = Pos;
					worldUp = Up;
					yaw = Yaw;
					pitch = Pitch;
					updateCameraVectors();
				}

				Camera(glm::vec3 Pos = glm::vec3(0.f), glm::vec3 Up = glm::vec3(0.f, 1.f, 0.f), float Yaw = YAW, float Pitch = PITCH) :
					front(glm::vec3(0.f, 0.f, -1.f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
				{
					position = Pos;
					worldUp = Up;
					yaw = Yaw;
					pitch = Pitch;
					updateCameraVectors();
				}

				Camera(f32 posX, f32 posY, f32 posZ, f32 upX, f32 upY, f32 upZ, f32 Yaw, f32 Pitch) :
					front(glm::vec3(0.f, 0.f, -1.f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
				{
					position = glm::vec3(posX, posY, posZ);
					worldUp = glm::vec3(upX, upY, upZ);
					yaw = Yaw;
					pitch = Pitch;
					updateCameraVectors();
				}

				glm::mat4 &GetViewMatrix()
				{
					m_View = glm::lookAt(position, position + front, up);
					return m_View;
				}

				glm::mat4 &GetProjectionMatrix()
				{
					m_Projection = glm::perspective(glm::radians(zoom), m_ScreenRes.x / m_ScreenRes.y, 0.1f, 1000.f);
					return m_Projection;
				}

				void ProcessKeyboard(CameraMovement dir, f64 dT, float speed = SPEED)
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

				void ProcessMouseMovement(f32 offsetX, f32 offsetY, bool constrainPitch = true)
				{
					offsetX *= mouseSensitivity;
					offsetY *= mouseSensitivity;

					yaw		+= offsetX;
					pitch	-= offsetY;

					if (constrainPitch)
					{
						if (pitch > 89.f)
							pitch = 89.f;
						if (pitch < -89.f)
							pitch = -89.f;
					}

					updateCameraVectors();
				}

				void ProcessMouseScroll(f32 offsetY)
				{
					if (zoom >= 1.f && zoom <= 45.f)
						zoom -= offsetY;
					if (zoom <= 1.f)
						zoom = 1.f;
					if (zoom >= 45.f)
						zoom = 45.f;
				}

				void RenderGUI()
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

				glm::vec3 position;
				glm::vec3 front;
				glm::vec3 up;
				glm::vec3 right;
				glm::vec3 worldUp;

				float yaw;
				float pitch;
				float movementSpeed;
				float mouseSensitivity;
				float zoom;

			private:
				glm::mat4 m_Projection;
				glm::mat4 m_View;
				glm::vec2 m_ScreenRes;

				void updateCameraVectors()
				{
					glm::vec3 Front;
					Front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
					Front.y = sin(glm::radians(pitch));
					Front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
					front	= glm::normalize(Front);

					right	= glm::normalize(glm::cross(front, worldUp));
					up		= glm::normalize(glm::cross(right, front));
				}
			};
		}
	}
}

#endif // !CAMERA_H
