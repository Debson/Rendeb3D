#ifndef MD_MATH
#define MD_MATH

#include <glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/quaternion.hpp>
#include <assimp/Importer.hpp> 
#include <assimp/postprocess.h>

namespace mdEngine
{
	namespace math
	{

		/*glm::mat3 aiMatrix3x3ToGlm(const aiMatrix3x3 &from);*/
        glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4 &from);
        glm::quat aiQuatToGlm(const glm::quat &from);
   }   
}

#endif // !MD_MATH



