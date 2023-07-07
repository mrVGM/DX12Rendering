#pragma once

#include "BaseObject.h"

#include <string>
#include <list>

namespace xml_reader
{
	struct Node;
}

namespace settings
{
	class SettingsReader : public BaseObject
	{
	protected:
		struct XMLNodes
		{
			std::list<xml_reader::Node*> m_rootNodes;
			std::list<xml_reader::Node*> m_allNodes;
			~XMLNodes();
		};
	
		void ParseSettingFile(const std::string& settingFilePath, XMLNodes& XMLNodes);
		const xml_reader::Node* FindSettingRootNode(const XMLNodes& XMLNodes);
	public:
		SettingsReader(const BaseObjectMeta& meta);
		virtual ~SettingsReader();
	};
}
