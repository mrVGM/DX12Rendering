#include "DataDefReader.h"

#include "DataDefReaderMeta.h"

#include "XMLReader.h"

#include "GeneratedObjectMeta.h"
#include "ScriptingStructMeta.h"

#include "BaseObjectContainer.h"

reflection::DataDefReader::DataDefReader() :
	SettingsReader(reflection::DataDefReaderMeta::GetInstance())
{
	xml_reader::Boot();
}

reflection::DataDefReader::~DataDefReader()
{
}

void reflection::DataDefReader::ParseXMLStruct(const std::string& filePath, StructTypePayload& payload)
{
	using namespace xml_reader;

	XMLNodes xmlNodes;
	ParseSettingFile(filePath, xmlNodes);

	const Node* dataDefNode = nullptr;
	for (auto it = xmlNodes.m_rootNodes.begin(); it != xmlNodes.m_rootNodes.end(); ++it)
	{
		const Node* cur = *it;
		if (cur->m_tagName == "data_def")
		{
			dataDefNode = cur;
			break;
		}
	}

	payload.FromXMLTree(*dataDefNode);
}
