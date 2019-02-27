#ifndef CAMERA_H
#define CAMERA_H

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "objects.h"
#include "types.h"
#include "gui.h"


namespace md
{
	namespace engine
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


		class Camera : public engine::Gui, public Object
		{
			CLASS_DECLARATION(Camera);

		public:
			Camera(glm::vec2 screenRes, glm::vec3 Pos = glm::vec3(0.f), glm::vec3 Up = glm::vec3(0.f, 1.f, 0.f), float Yaw = YAW, float Pitch = PITCH);

			Camera(glm::vec3 Pos = glm::vec3(0.f), glm::vec3 Up = glm::vec3(0.f, 1.f, 0.f), float Yaw = YAW, float Pitch = PITCH);

			Camera(f32 posX, f32 posY, f32 posZ, f32 upX, f32 upY, f32 upZ, f32 Yaw, f32 Pitch);

			glm::mat4 &GetViewMatrix();

			glm::mat4 &GetProjectionMatrix();			
			
			void ProcessKeyboard(CameraMovement dir, f64 dT, float speed = SPEED);

			void ProcessMouseMovement(f32 offsetX, f32 offsetY, bool constrainPitch = true);

			void ProcessMouseScroll(f32 offsetY);

			void RenderGUI();

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

			void updateCameraVectors();
		};
		
	}
}

#endif // !CAMERA_H
