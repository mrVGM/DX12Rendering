#pragma once

#include <string>

namespace xml_reader
{
	class Node;
}

namespace collada
{
	class Scene;
	bool ReadGeometry(const std::string& id, const xml_reader::Node* geometry, bool invertAxis, Scene& scene);
	bool ShouldInvertAxis(const xml_reader::Node* rootDataNode);
}