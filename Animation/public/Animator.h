#pragma once

#include "BaseObject.h"

#include <string>

namespace animation
{
	class Animator : public BaseObject
	{
	private:
		std::string m_objectName;

	public:
		Animator(const std::string& objectName);
		virtual ~Animator();
	};
}