#pragma once

#include "BaseObjectMeta.h"

namespace xml_reader
{
	class XMLReaderMeta : public BaseObjectMeta
	{
	public:
		static const XMLReaderMeta& GetInstance();
		XMLReaderMeta();
	};
}