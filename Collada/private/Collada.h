#pragma once

#include "symbol.h"

#include "XMLReader.h"
#include "ColladaEntities.h"

#include <map>
#include <list>
#include <string>

namespace collada
{
	bool ConvertToScene(const std::list<xml_reader::Node*>& nodes, Scene& scene);
}