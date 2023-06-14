#pragma once

#include "symbol.h"

#include <map>
#include <list>
#include <string>

namespace xml_reader
{
	struct Node
	{
		scripting::ISymbol* m_rootSymbol;

		std::string m_tagName;
		std::map<std::string, std::string> m_tagProps;

		std::list<scripting::ISymbol*> m_data;

		std::list<Node*> m_children;
	};

	struct IXMLReader
	{
		virtual scripting::ISymbol* ReadColladaFile(const std::string& file) = 0;
		virtual bool ConstructColladaTree(scripting::ISymbol* rootSymbol, std::list<Node*>& nodes, std::list<Node*>& allNodes) = 0;
	};

	IXMLReader* GetReader();
	void Boot();
}