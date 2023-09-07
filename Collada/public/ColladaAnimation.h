#pragma once

#include "BaseObject.h"
#include "ColladaEntities.h"

#include <string>
#include <vector>

namespace collada
{
	class ColladaAnimation : public BaseObject
	{
	private:
		Animation m_animation;
	public:
		ColladaAnimation();
		virtual ~ColladaAnimation();

		bool Load(const std::string& filePath);
	};
}