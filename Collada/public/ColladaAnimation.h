#pragma once

#include "BaseObject.h"
#include "ColladaEntities.h"

#include <string>
#include <vector>

namespace collada
{
	class ColladaAnimation : public BaseObject
	{
	public:
		ColladaAnimation();
		virtual ~ColladaAnimation();

		bool Load(const std::string& filePath);
	};
}