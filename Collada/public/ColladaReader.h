#pragma once

#include "BaseObject.h"

#include "symbol.h"

#include <map>
#include <list>
#include <string>

namespace collada
{
	struct ColladaNode
	{
		scripting::ISymbol* m_rootSymbol;

		std::string m_tagName;
		std::map<std::string, std::string> m_tagProps;

		std::list<scripting::ISymbol*> m_data;

		std::list<ColladaNode*> m_children;
	};

	class ColladaReader : public BaseObject
	{
	private:
		ColladaReader();
	public:
		static ColladaReader& GetInstance();

		scripting::ISymbol* ReadColladaFile(const std::string& file);
		bool ConstructColladaTree(scripting::ISymbol* rootSymbol, std::list<collada::ColladaNode*>& nodes, std::list<collada::ColladaNode*>& allNodes);

		virtual ~ColladaReader();
	};
}