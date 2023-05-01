#pragma once

#include "symbol.h"

#include <map>
#include <list>
#include <string>

namespace collada
{
	struct ColladaNode;
	struct Scene;

	struct IColladaReader
	{
		virtual scripting::ISymbol* ReadColladaFile(const std::string& file) = 0;
		virtual bool ConstructColladaTree(scripting::ISymbol* rootSymbol, std::list<collada::ColladaNode*>& nodes, std::list<collada::ColladaNode*>& allNodes) = 0;
	};

	IColladaReader* GetReader();
	void ReleaseColladaReader();

	bool ConvertToScene(const std::list<collada::ColladaNode*>& nodes, Scene& scene);
}