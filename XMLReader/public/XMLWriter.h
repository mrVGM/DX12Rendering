#pragma once

#include "symbol.h"

#include <map>
#include <list>
#include <string>

namespace xml_writer
{
	struct Node
	{
		std::string m_tagName;
		std::map<std::string, std::string> m_tagProps;
		std::list<Node> m_children;
		std::string m_content;

		std::string ToString();
	};

	std::string EncodeAsString(const std::string& str);
}