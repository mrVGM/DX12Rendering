#pragma once

#include "SettingsReader.h"

#include "BaseObjectMetaTag.h"

#include "Type.h"

#include <string>
#include <map>

namespace reflection
{
	class DataDefReader : public settings::SettingsReader
	{
	public:
		DataDefReader();
		virtual ~DataDefReader();

		StructType* ParseXMLStruct(const std::string& filePath);
	};
}
