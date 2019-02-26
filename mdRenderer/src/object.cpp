#include "objects.h"

#include <unordered_map>
#include <algorithm>

namespace md
{
	typedef std::unordered_map<std::string, engine::Object*> ObjectsMap;
	namespace engine
	{
		ObjectsMap mdObjectsMap;

	}

	u32 engine::Object::ObjectCounter = 0;

	engine::Object::Object(std::string const &name) : Component(name)
	{
		OnCreate(name);
	}

	u32 engine::Object::GetInstanceID()
	{
		return m_InstanceID;
	}

	std::string engine::Object::ToString()
	{
		return m_Name;
	}

	void engine::Object::Destroy(Object *obj, f32 time)
	{
		delete &obj;
	}

	engine::Object *engine::Object::FindObjectOfName(std::string name)
	{
		return mdObjectsMap.at(name);
	}

	bool engine::Object::operator==(const Object & rhs) const
	{
		return true;
	}

	void engine::Object::OnCreate(std::string const &name)
	{
		m_Name = name;
		m_InstanceID = ObjectCounter;
		//mdObjectsMap.insert(std::make_pair(name, this));
		++ObjectCounter;
	}

}