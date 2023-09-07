#pragma once

#include "symbol.h"

#include "DataLib.h"
#include "MemoryFile.h"

#include <string>
#include <list>
#include <map>

namespace collada
{
	struct Matrix
	{
		float m_coefs[16] = 
		{
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1,
		};
		
		static int GetIndex(int row, int column);
		static Matrix Multiply(const Matrix& m1, const Matrix& m2);

		static const Matrix& One();
		static const Matrix& Zero();

		float GetCoef(int row, int column) const;
		Matrix Transpose() const;
	};


	struct Vector3
	{
		float m_values[3];
	};

	struct Vector2
	{
		float m_values[2];
	};

	struct Vertex
	{
		float m_position[3] = {};
		float m_normal[3] = {};
		float m_uv[2] = {};

		bool Equals(const Vertex& other) const;
	};
	
	struct SkeletalMeshVertexWeights
	{
		int m_joints[4] = { -1, -1, -1, -1 };
		float m_weights[4] = { -1, -1, -1, -1 };
	};

	struct MaterialIndexRange
	{
		std::string m_name;
		int indexOffset = -1;
		int indexCount = -1;
	};

	struct Geometry
	{
		std::list<Vector3> m_vertexPositions;
		std::list<Vertex> m_vertices;
		std::list<int> m_indices;
		std::list<MaterialIndexRange> m_materials;

		void Serialize(data::MemoryFileWriter& writer, int id);
		void Deserialize(data::MemoryFileReader& reader, int& id);
	};

	struct Skeleton
	{
		struct VertexWeight
		{
			std::string m_joint;
			float m_weight = 0;
		};

		std::vector<std::string> m_joints;
		std::vector<int> m_jointsParents;
		std::vector<Matrix> m_jointTransforms;
		Matrix m_bindShapeMatrix;
		std::map<std::string, Matrix> m_invertBindMatrices;
		std::list<std::list<VertexWeight>> m_weights;

		void Serialize(data::MemoryFileWriter& writer, int id);
		void Deserialize(data::MemoryFileReader& reader, int& id);
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

		void Serialize(data::MemoryFileWriter& writer, int id);
		void Deserialize(data::MemoryFileReader& reader, int& id);
	};

	struct SkeletonBuffer
	{
		Matrix m_bindPoseMatrix;
		std::list<Matrix> m_invBindPoseMatrices;
	};

	struct SkeletonPoseBuffer
	{
		std::list<Matrix> m_jointTransforms;
	};

	struct VertexWeightsBuffer
	{
		std::list<SkeletalMeshVertexWeights> m_weights;
	};

	struct InstanceBuffer
	{
		std::list<GeometryInstanceData> m_data;
	};

	struct ColladaMaterial
	{
		std::string m_name;
		float m_diffuseColor[4] = {};
		float m_specularColor[4] = {};
	};

	struct Scene
	{
		std::map<std::string, Geometry> m_geometries;
		std::map<std::string, Skeleton> m_skeletons;
		std::map<std::string, Object> m_objects;
		std::map<std::string, int> m_objectInstanceMap;
		std::map<std::string, InstanceBuffer> m_instanceBuffers;
		std::map<std::string, ColladaMaterial> m_materials;
		std::map<std::string, SkeletonBuffer> m_skeletonBuffers;
		std::map<std::string, SkeletonPoseBuffer> m_skeletonPoseBuffers;
		std::map<std::string, VertexWeightsBuffer> m_vertexWeightsBuffers;

		void ConstructInstanceBuffers();
		void ConstructSkeletonBuffers();

		void Serialize(data::MemoryFileWriter& writer);
		void Deserialize(data::MemoryFileReader& reader);
	};

	struct KeyFrame
	{
		float m_time;
		collada::Matrix m_transform;
		std::string m_interpolation;

		void Serialize(data::MemoryFileWriter& writer);
		void Deserialize(data::MemoryFileReader& reader);
	};

	struct AnimChannel
	{
		std::string m_boneName;
		std::vector<KeyFrame> m_keyFrames;

		void Serialize(data::MemoryFileWriter& writer);
		void Deserialize(data::MemoryFileReader& reader);
	};

	struct Animation
	{
		std::vector<std::string> m_bones;
		std::vector<int> m_boneParents;

		std::map<std::string, AnimChannel> m_channels;
	};
}