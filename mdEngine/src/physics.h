#ifndef PHYSICS_H
#define PHYSICS_H

#include <glm.hpp>

#include "types.h"

namespace md
{
	namespace engine
	{
		namespace physics
		{

			class Transform
			{
			public:
				Transform();
				virtual ~Transform();

				glm::vec3 position;
				glm::vec3 localScale;
				void Rotate(f32 xAngle, f32 yAngle, f32 zAngle);
				void Rotate(glm::vec3 axis, f32 angle);
				void updateMatrices(glm::mat4 &model);

				glm::mat4 matrixModel;
			protected:
				b8 hasChanged();
			private:
				glm::vec3 m_RotationAngle;
				glm::mat4 m_RotateModel;
				glm::vec3 m_PrevPos;
				glm::vec3 m_PrevScale;
				bool m_Rotated;
			};

		}
	}

	namespace mdPhysics = engine::physics;
}


#endif // !PHYSICS_H
