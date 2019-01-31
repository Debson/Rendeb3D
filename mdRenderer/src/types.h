#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <glm.hpp>


namespace md
{

	namespace core
	{
		typedef bool b8;

		typedef int8_t		s8;
		typedef int16_t		s16;
		typedef int32_t		s32;
		typedef int64_t		s64;

		typedef uint8_t		u8;
		typedef uint16_t	u16;
		typedef uint32_t	u32;
		typedef uint64_t	u64;

		typedef float		f32;
		typedef double		f64;
	}
	using namespace core;

	namespace color
	{
		const glm::vec3 White(1.f);
		const glm::vec3 Black(0.f);
		const glm::vec3 Grey(0.85f);
		const glm::vec3 DarkGrey(0.5f);
		const glm::vec3 Red(1.f, 0.f, 0.f);
		const glm::vec3 Green(0.f, 1.f, 0.f);
		const glm::vec3 Blue(0.f, 0.f, 1.f);
		const glm::vec3 Pink(1.f, 0.0784f, 0.576f);
		const glm::vec3 Azure(240.f / 255.f, 1.f, 1.f);
		const glm::vec3 Silver(192.f / 255.f);
		const glm::vec3 Orange(1.f, 140.f / 255.f, 0);
		const glm::vec3 WhiteGlow(20.f);
	}

}

#endif // !TYPES_H
