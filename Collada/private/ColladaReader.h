#pragma once

#include "BaseObject.h"

#include "symbol.h"

#include <map>
#include <list>
#include <string>

namespace collada
{
	struct ColladaNode;

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