#pragma once

#include "BaseObject.h"

#include <string>

namespace animation
{
	class AnimatorUpdater;

	class Animator : public BaseObject
	{
	private:
		std::string m_objectName;
		AnimatorUpdater* m_updater = nullptr;

	public:
		Animator(const std::string& objectName);
		virtual ~Animator();
	};
}