#pragma once

#include "symbol.h"

#include "DataLib.h"
#include "MemoryFile.h"

#include <string>
#include <list>
#include <map>

namespace collada
{
	struct Vertex
	{
		float m_position[3] = {};
		float m_normal[3] = {};
		float m_uv[2] = {};

		bool Equals(const Vertex& other) const;
	};

	struct MaterialIndexRange
	{
		std::string m_name;
		int indexOffset = -1;
		int indexCount = -1;
	};

	struct Geometry
	{
		std::list<Vertex> m_vertices;
		std::list<int> m_indices;
		std::list<MaterialIndexRange> m_materials;

		void Serialize(data::MemoryFileWriter& writer);
		void Deserialize(data::MemoryFileReader& reader);
	};

	struct GeometryInstanceData
	{
		float m_position[3];
		float m_rotation[4];
		float m_scale[3];
	};

	struct Object
	{
		float m_transform[16];
		GeometryInstanceData m_instanceData;

		std::string m_geometry;
		std::list<std::string> m_materialOverrides;

		void CalcPositionRotationScale(bool invertAxis);
		void InvertAxis();

		void Serialize(data::MemoryFileWriter& writer);
		void Deserialize(data::MemoryFileReader& reader);
	};

	struct InstanceBuffer
	{
		std::list<GeometryInstanceData> m_data;
	};

	struct ColladaMaterial
	{
		std::string m_name;
		float m_diffuseColor[4];
	};

	struct Scene
	{
		std::map<std::string, Geometry> m_geometries;
		std::map<std::string, Object> m_objects;
		std::map<std::string, int> m_objectInstanceMap;
		std::map<std::string, InstanceBuffer> m_instanceBuffers;
		std::map<std::string, ColladaMaterial> m_materials;

		void ConstructInstanceBuffers();
	};
}