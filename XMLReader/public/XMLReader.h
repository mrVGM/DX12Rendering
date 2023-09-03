#pragma once

#include "symbol.h"

#include <map>
#include <list>
#include <string>
#include <functional>

namespace xml_reader
{
	struct Node
	{
		scripting::ISymbol* m_rootSymbol;

		std::string m_tagName;
		std::map<std::string, std::string> m_tagProps;

		std::list<scripting::ISymbol*> m_data;

		Node* m_parent = nullptr;
		std::list<Node*> m_children;
	};

	struct IXMLReader
	{
		virtual scripting::ISymbol* ReadColladaFile(const std::string& file) = 0;
		virtual bool ConstructColladaTree(scripting::ISymbol* rootSymbol, std::list<Node*>& nodes, std::list<Node*>& allNodes) = 0;
	};

	IXMLReader* GetReader();
	void Boot();
	void FindChildNodes(const Node* rootNode, std::function<bool(const Node*)> predicate, std::list<const Node*>& nodesFound);
	const Node* FindChildNode(const Node* rootNode, std::function<bool(const Node*)> predicate);
	
}