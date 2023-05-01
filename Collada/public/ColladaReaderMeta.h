#pragma once

#include "BaseObjectMeta.h"

namespace collada
{
	class ColladaReaderMeta : public BaseObjectMeta
	{
	public:
		static const ColladaReaderMeta& GetInstance();
		ColladaReaderMeta();
	};
}