#pragma once

#include "SettingsReader.h"

#include "BaseObjectMetaTag.h"

#include "SerializeToStringUtils.h"

#include <string>
#include <map>

namespace reflection
{
	class DataDefReader : public settings::SettingsReader
	{
	public:
		DataDefReader();
		virtual ~DataDefReader();

		void ParseXMLStruct(const std::string& filePath, StructTypePayload& payload);
	};
}
