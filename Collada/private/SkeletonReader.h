#pragma once

#include <string>
#include <vector>

namespace xml_reader
{
	struct Node;
}

namespace collada
{
	class Scene;
	class Matrix;

	class Skeleton
	{
	private:
		Scene& m_scene;

	public:
		std::vector<std::string> m_joints;
		std::vector<Matrix> m_invertBindMatrices;

		Skeleton(Scene& scene);
		Skeleton(const Skeleton& other) = delete;
		Skeleton& operator=(const Skeleton& other) = delete;

		bool ReadFromNode(const xml_reader::Node* node, const xml_reader::Node* containerNode);
	};
}