#pragma once

#include "BaseObjectMeta.h"

namespace reflection
{
	class DataDefReaderMeta : public BaseObjectMeta
	{
	public:
		static const DataDefReaderMeta& GetInstance();
		DataDefReaderMeta();
	};
}