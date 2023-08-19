#pragma once

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
		Skeleton(Scene& scene);
		Skeleton(const Skeleton& other) = delete;
		Skeleton& operator=(const Skeleton& other) = delete;

		bool ReadFromNode(const xml_reader::Node* node, const xml_reader::Node* containerNode);
	};
}