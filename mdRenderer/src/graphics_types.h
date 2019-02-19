#ifndef GRAPHICS_TYPES_H
#define GRAPHICS_TYPES_H

#include "types.h"

namespace md
{
	namespace engine
	{
#define MD_GREATER 0x100
#define MD_LESS 0x101

		namespace graphics
		{
			union type_t
			{
				f32 f;
				s32 i;
				b8 b : 1;

				inline bool operator==(const type_t &rhs)
				{
					if(this->f == rhs.f || this->i == rhs.i || this->b == rhs.b)
						return true;
					return false;
				}
			};

			enum TransitionType : u8
			{
				BILATERAL,
				ONESIDED
			};
		}
	}
}

#endif // !GRAPHICS_TYPES_H

