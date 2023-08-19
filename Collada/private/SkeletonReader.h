#pragma once

namespace xml_reader
{
	struct Node;
}

namespace collada
{
	struct Skeleton
	{
		bool ReadFromNode(const xml_reader::Node* node, const xml_reader::Node* containerNode);
	};
}