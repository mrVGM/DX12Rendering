#include "ColladaEntities.h"

#include "DataLib.h"

namespace
{
	struct MaterialRangeSerializable
	{
		int m_nameIndex = -1;
		int m_indexOffset = -1;
		int m_indexCount = -1;
	};
}

void collada::Geometry::Serialize(data::BinWriter& file)
{
	std::map<std::string, int> nameIds;

	for (auto it = m_materials.begin(); it != m_materials.end(); ++it)
	{
		const MaterialIndexRange& curMaterial = *it;
		nameIds[curMaterial.m_name] = 0;
	}

	{
		data::BinChunk namesChunk;

		unsigned int size = sizeof(unsigned int);

		std::vector<std::string> names;
		int index = 0;
		for (auto it = nameIds.begin(); it != nameIds.end(); ++it)
		{
			names.push_back(it->first);
			it->second = index++;

			size += it->first.size() + 1;
		}

		namesChunk.m_size = size;
		namesChunk.m_data = new char[size];

		memset(namesChunk.m_data, 0, size);

		void* curPtr = namesChunk.m_data;
		{
			unsigned int* countNames = static_cast<unsigned int*>(curPtr);
			*countNames = names.size();
			++countNames;
			curPtr = countNames;
		}

		{
			char* namePtr = static_cast<char*>(curPtr);
			for (auto it = names.begin(); it != names.end(); ++it)
			{
				const std::string& curName = *it;
				memcpy(namePtr, curName.c_str(), curName.size());
				namePtr += curName.size() + 1;
			}
		}

		namesChunk.Write(file);
	}

	{
		data::BinChunk verticesChunk;
		verticesChunk.m_size = m_vertices.size() * sizeof(Vertex);
		verticesChunk.m_data = new char[verticesChunk.m_size];

		Vertex* curData = reinterpret_cast<Vertex*>(verticesChunk.m_data);
		for (auto it = m_vertices.begin(); it != m_vertices.end(); ++it)
		{
			*curData = *it;
			++curData;
		}

		verticesChunk.Write(file);
	}

	{
		data::BinChunk indicesChunk;
		indicesChunk.m_size = m_indices.size() * sizeof(int);
		indicesChunk.m_data = new char[indicesChunk.m_size];

		int* curData = reinterpret_cast<int*>(indicesChunk.m_data);
		for (auto it = m_indices.begin(); it != m_indices.end(); ++it)
		{
			*curData = *it;
			++curData;
		}

		indicesChunk.Write(file);
	}
	
	{
		data::BinChunk materialsChunk;

		materialsChunk.m_size = m_materials.size() * sizeof(MaterialRangeSerializable);
		materialsChunk.m_data = new char[materialsChunk.m_size];

		MaterialRangeSerializable* curData = reinterpret_cast<MaterialRangeSerializable*>(materialsChunk.m_data);
		for (auto it = m_materials.begin(); it != m_materials.end(); ++it)
		{
			const MaterialIndexRange& curMaterialRange = *it;

			*curData = MaterialRangeSerializable
			{ 
				nameIds[curMaterialRange.m_name],
				curMaterialRange.indexOffset,
				curMaterialRange.indexCount
			};

			++curData;
		}

		materialsChunk.Write(file);
	}
}

void collada::Geometry::Deserialize(data::BinReader& file)
{
	std::vector<std::string> names;

	{
		data::BinChunk namesChunk;
		namesChunk.Read(file);


		unsigned int* namesCount = reinterpret_cast<unsigned int*>(namesChunk.m_data);

		char* namesPtr = reinterpret_cast<char*>(namesCount + 1);

		for (unsigned int i = 0; i < *namesCount; ++i)
		{
			std::string& tmp = names.emplace_back();
			tmp = namesPtr;

			namesPtr += tmp.size() + 1;
		}
	}

	{
		data::BinChunk verticesChunk;
		verticesChunk.Read(file);

		int vertexCount = verticesChunk.m_size / sizeof(Vertex);

		Vertex* vertexPtr = reinterpret_cast<Vertex*>(verticesChunk.m_data);
		for (int i = 0; i < vertexCount; ++i)
		{
			Vertex& tmp = m_vertices.emplace_back();
			tmp = *vertexPtr;
			++vertexPtr;
		}
	}

	{
		data::BinChunk indicesChunk;
		indicesChunk.Read(file);

		int indexCount = indicesChunk.m_size / sizeof(int);

		int* indexPtr = reinterpret_cast<int*>(indicesChunk.m_data);
		for (int i = 0; i < indexCount; ++i)
		{
			int& tmp = m_indices.emplace_back();
			tmp = *indexPtr;
			++indexPtr;
		}
	}

	{
		data::BinChunk materialsChunk;
		materialsChunk.Read(file);

		int materialsCount = materialsChunk.m_size / sizeof(MaterialRangeSerializable);

		MaterialRangeSerializable* materialPtr = reinterpret_cast<MaterialRangeSerializable*>(materialsChunk.m_data);
		for (int i = 0; i < materialsCount; ++i)
		{
			MaterialIndexRange& tmp = m_materials.emplace_back();

			tmp.m_name = names[materialPtr->m_nameIndex];
			tmp.indexOffset = materialPtr->m_indexOffset;
			tmp.indexCount = materialPtr->m_indexCount;
			++materialPtr;
		}
	}
}