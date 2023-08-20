#pragma once

#include "ColladaEntities.h"

#include <string>
#include <vector>
#include <map>

namespace xml_reader
{
	struct Node;
}

namespace collada
{
	class Scene;

	class Skeleton
	{
	private:
		Scene& m_scene;

	public:
		Matrix m_bindShapeMatrix;
		std::vector<std::string> m_joints;
		std::vector<Matrix> m_invertBindMatrices;
		std::vector<std::map<std::string, float>> m_weights;
		
		Skeleton(Scene& scene);
		Skeleton(const Skeleton& other) = delete;
		Skeleton& operator=(const Skeleton& other) = delete;

		bool ReadFromNode(const xml_reader::Node* node, const xml_reader::Node* containerNode);
	};
}