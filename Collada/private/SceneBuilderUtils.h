#pragma once

#include <string>

namespace xml_reader
{
	class Node;
}

namespace collada
{
	class Scene;
	struct Matrix;
	struct Object;

	bool ReadGeometry(const std::string& id, const xml_reader::Node* geometry, bool invertAxis, Scene& scene);
	Object* ReadObject(const xml_reader::Node* node, const std::string& geoName, bool invertAxis, Scene& scene);
	bool ShouldInvertAxis(const xml_reader::Node* rootDataNode);
	void ReadMatricesFromNode(const xml_reader::Node* node, collada::Matrix* matrices, int numMatrices);
}