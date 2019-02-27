#ifndef SCRIPTABLE_H
#define SCRIPTABLE_H

namespace md
{
	class Script
	{
	public:

		virtual ~Script() { }

		virtual void Start() = 0;

		virtual void Update() = 0;

	private:
	};
}

#endif // !SCRIPTABLE_H

