#pragma once

#include "BaseObject.h"

#include "ColladaEntities.h"

#include <map>
#include <string>

namespace animation
{
	class AnimRepo : public BaseObject
	{
	private:
		std::map<std::string, collada::Animation> m_animations;

	public:
		AnimRepo();
		virtual ~AnimRepo();

		void LoadAnimations();

		const collada::Animation* GetAnimation(const std::string& animationName);
	};
}