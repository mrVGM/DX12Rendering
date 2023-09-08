#pragma once

#include <string>
#include <list>

namespace xml_reader
{
	class Node;
}

namespace collada
{
	class Scene;
	struct Matrix;
	struct Object;

	struct ColladaNodesContainer
	{
		std::list<xml_reader::Node*> m_nodes;
		~ColladaNodesContainer();
	};

	bool ReadGeometry(const std::string& id, const xml_reader::Node* geometry, bool invertAxis, Scene& scene);
	Object* ReadObject(const xml_reader::Node* node, const std::string& geoName, bool invertAxis, Scene& scene);
	bool ShouldInvertAxis(const xml_reader::Node* rootDataNode);
	void ReadMatricesFromNode(const xml_reader::Node* node, Matrix* matrices, int numMatrices);
	bool GetLocalTransformNode(const xml_reader::Node* node, Matrix& matrix);
	collada::Matrix ChangeAxisOfMatrix(const collada::Matrix& matrix);
}