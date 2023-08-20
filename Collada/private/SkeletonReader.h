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

	class SkeletonReader
	{
	private:
		Scene& m_scene;

	public:
		Matrix m_bindShapeMatrix;
		std::vector<std::string> m_joints;
		std::vector<Matrix> m_invertBindMatrices;
		std::vector<std::map<std::string, float>> m_weights;
		
		SkeletonReader(Scene& scene);
		SkeletonReader(const SkeletonReader& other) = delete;
		SkeletonReader& operator=(const SkeletonReader& other) = delete;

		bool ReadFromNode(const xml_reader::Node* node, const xml_reader::Node* containerNode, std::string& geometryName);

		void ToSkeleton(collada::Skeleton& skeleton);
	};
}