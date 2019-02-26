#include "component.h"

#include "objects.h"
#include "animator.h"

CLASS_DEFINITION(md::engine::Object, md::engine::Graphics);
CLASS_DEFINITION(md::engine::Object, md::engine::Animator);


namespace md
{
	const std::size_t engine::Component::Type = std::hash<std::string>()(TO_STRING(engine::Component));

	namespace engine
	{
	}
}