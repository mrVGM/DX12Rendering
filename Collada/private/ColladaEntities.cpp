#include "ColladaEntities.h"

#include "DataLib.h"
#include "MemoryFile.h"

#include <corecrt_math_defines.h>

namespace
{
	struct MaterialRangeSerializable
	{
		int m_nameIndex = -1;
		int m_indexOffset = -1;
		int m_indexCount = -1;
	};

	collada::Matrix m_one =
	{
		{
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1
		}
	};
	collada::Matrix m_zero = { {} };
}

void collada::Geometry::Serialize(data::MemoryFileWriter& writer, int id)
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

		namesChunk.Write(writer);
	}

	{
		data::BinChunk idChunk;

		idChunk.m_size = sizeof(int);
		idChunk.m_data = new char[sizeof(int)];

		int* idPtr = reinterpret_cast<int*>(idChunk.m_data);
		*idPtr = id;

		idChunk.Write(writer);
	}

	{
		data::BinChunk vertexPositionsChunk;
		vertexPositionsChunk.m_size = m_vertexPositions.size() * sizeof(Vector3);
		vertexPositionsChunk.m_data = new char[vertexPositionsChunk.m_size];

		Vector3* curData = reinterpret_cast<Vector3*>(vertexPositionsChunk.m_data);
		for (auto it = m_vertexPositions.begin(); it != m_vertexPositions.end(); ++it)
		{
			*curData = *it;
			++curData;
		}

		vertexPositionsChunk.Write(writer);
	}

	{
		data::BinChunk vertexPositionIdChunk;
		vertexPositionIdChunk.m_size = m_vertexPositionId.size() * sizeof(int);
		vertexPositionIdChunk.m_data = new char[vertexPositionIdChunk.m_size];

		int* curData = reinterpret_cast<int*>(vertexPositionIdChunk.m_data);
		for (auto it = m_vertexPositionId.begin(); it != m_vertexPositionId.end(); ++it)
		{
			*curData = *it;
			++curData;
		}

		vertexPositionIdChunk.Write(writer);
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

		verticesChunk.Write(writer);
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

		indicesChunk.Write(writer);
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

		materialsChunk.Write(writer);
	}
}

void collada::Geometry::Deserialize(data::MemoryFileReader& reader, int& id)
{
	std::vector<std::string> names;

	{
		data::BinChunk namesChunk;
		namesChunk.Read(reader);


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
		data::BinChunk idChunk;
		idChunk.Read(reader);

		id = *reinterpret_cast<int*>(idChunk.m_data);
	}

	{
		data::BinChunk vertexPositionsChunk;
		vertexPositionsChunk.Read(reader);

		unsigned int numPositions = vertexPositionsChunk.m_size / sizeof(Vector3);
		Vector3* curPosition = reinterpret_cast<Vector3*>(vertexPositionsChunk.m_data);
		for (unsigned int i = 0; i < numPositions; ++i)
		{
			m_vertexPositions.push_back(curPosition[i]);
		}
	}

	{
		data::BinChunk vertexPositionIdChunk;
		vertexPositionIdChunk.Read(reader);

		int numIds = vertexPositionIdChunk.m_size / sizeof(int);
		int* data = reinterpret_cast<int*>(vertexPositionIdChunk.m_data);

		for (int i = 0; i < numIds; ++i)
		{
			m_vertexPositionId.push_back(data[i]);
		}
	}

	{
		data::BinChunk verticesChunk;
		verticesChunk.Read(reader);

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
		indicesChunk.Read(reader);

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
		materialsChunk.Read(reader);

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

void collada::Scene::ConstructInstanceBuffers()
{
	m_instanceBuffers.clear();
	m_objectInstanceMap.clear();

	for (std::map<std::string, Geometry>::const_iterator it = m_geometries.begin();
		it != m_geometries.end(); ++it) {
		m_instanceBuffers.insert(std::pair<std::string, InstanceBuffer>(it->first, InstanceBuffer()));
	}

	for (std::map<std::string, Object>::const_iterator it = m_objects.begin();
		it != m_objects.end(); ++it) {
		InstanceBuffer& cur = m_instanceBuffers[it->second.m_geometry];

		cur.m_data.push_back(it->second.m_instanceData);
		m_objectInstanceMap[it->first] = cur.m_data.size() - 1;
	}
}

namespace
{
	const std::list<collada::Skeleton::VertexWeight>* GetWeightsForVertex(
		const collada::Geometry& geo,
		const collada::Skeleton& skeleton,
		int vertexPositionId)
	{
		using namespace collada;

		float eps = 0.000001;

		const std::list<collada::Skeleton::VertexWeight>* res = nullptr;

		auto weightsIt = skeleton.m_weights.begin();
		for (int i = 0; i < vertexPositionId; ++i)
		{
			++weightsIt;
		}
		res = &(*weightsIt);
		return res;
	}
}

void collada::Scene::ConstructSkeletonBuffers()
{
	m_skeletonBuffers.clear();
	m_skeletonPoseBuffers.clear();
	m_vertexWeightsBuffers.clear();

	for (auto it = m_skeletons.begin();
		it != m_skeletons.end(); ++it)
	{
		{
			m_skeletonBuffers[it->first] = SkeletonBuffer();
			SkeletonBuffer& curBuffer = m_skeletonBuffers[it->first];
			curBuffer.m_bindPoseMatrix = it->second.m_bindShapeMatrix;

			for (auto jointIt = it->second.m_joints.begin(); jointIt != it->second.m_joints.end(); ++jointIt)
			{
				curBuffer.m_invBindPoseMatrices.push_back(it->second.m_invertBindMatrices[*jointIt]);
			}
		}

		{
			const Geometry& geo = m_geometries[it->first];

			std::map<std::string, int> jointIDs;
			{
				int index = 0;
				for (auto jointIt = it->second.m_joints.begin(); jointIt != it->second.m_joints.end(); ++jointIt)
				{
					jointIDs[*jointIt] = index++;
				}
			}

			m_vertexWeightsBuffers[it->first] = VertexWeightsBuffer();
			VertexWeightsBuffer& curBuffer = m_vertexWeightsBuffers[it->first];

			auto vertexPositionIdIt = geo.m_vertexPositionId.begin();
			for (auto vertexIt = geo.m_vertices.begin(); vertexIt != geo.m_vertices.end(); ++vertexIt)
			{
				int vertexPositionId = *vertexPositionIdIt++;

				SkeletalMeshVertexWeights& curSkeletalMeshVertex = curBuffer.m_weights.emplace_back();
				const std::list<collada::Skeleton::VertexWeight>* weights = GetWeightsForVertex(geo, it->second, vertexPositionId);

				if (!weights)
				{
					continue;
				}

				int index = 0;
				for (auto weightIt = weights->begin(); weightIt != weights->end(); ++weightIt)
				{
					if (index >= 8)
					{
						break;
					}

					curSkeletalMeshVertex.m_joints[index] = jointIDs[(*weightIt).m_joint];
					curSkeletalMeshVertex.m_weights[index] = (*weightIt).m_weight;

					++index;
				}
			}
		}
	}

	for (auto it = m_objects.begin();
		it != m_objects.end(); ++it)
	{

		auto skeletonIt = m_skeletons.find(it->second.m_geometry);
		if (skeletonIt == m_skeletons.end())
		{
			continue;
		}

		const Skeleton& skeleton = skeletonIt->second;

		m_skeletonPoseBuffers[it->first] = SkeletonPoseBuffer();
		SkeletonPoseBuffer& poseBuffer = m_skeletonPoseBuffers[it->first];

		for (int i = 0; i < skeleton.m_joints.size(); ++i)
		{
			Matrix curTransform = skeleton.m_jointTransforms[i];
			int curParent = skeleton.m_jointsParents[i];

			while (curParent >= 0)
			{
				curTransform = Matrix::Multiply(curTransform, skeleton.m_jointTransforms[curParent]);
				curParent = skeleton.m_jointsParents[curParent];
			}

			poseBuffer.m_jointTransforms.push_back(curTransform);
		}
	}
}

void collada::Scene::Serialize(data::MemoryFileWriter& writer)
{
	std::map<std::string, int> nameIds;

	for (auto it = m_geometries.begin(); it != m_geometries.end(); ++it)
	{
		const std::string& geometryName = it->first;
		nameIds[geometryName] = 0;
	}

	for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
	{
		const std::string& objectName = it->first;
		nameIds[objectName] = 0;
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

		namesChunk.Write(writer);
	}

	{
		data::BinChunk countsChunk;
		countsChunk.m_size = 3 * sizeof(unsigned int);

		countsChunk.m_data = new char[countsChunk.m_size * sizeof(unsigned int)];
		unsigned int* geoCount = reinterpret_cast<unsigned int*>(countsChunk.m_data);
		unsigned int* objCount = geoCount + 1;
		unsigned int* skeletonsCount = geoCount + 2;

		*geoCount = m_geometries.size();
		*objCount = m_objects.size();
		*skeletonsCount = m_skeletons.size();

		countsChunk.Write(writer);
	}

	for (auto it = m_geometries.begin(); it != m_geometries.end(); ++it)
	{
		it->second.Serialize(writer, nameIds[it->first]);
	}

	for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
	{
		it->second.Serialize(writer, nameIds[it->first]);
	}

	for (auto it = m_skeletons.begin(); it != m_skeletons.end(); ++it)
	{
		it->second.Serialize(writer, nameIds[it->first]);
	}
}

void collada::Scene::Deserialize(data::MemoryFileReader& reader)
{
	std::vector<std::string> names;

	{
		data::BinChunk namesChunk;
		namesChunk.Read(reader);


		unsigned int* namesCount = reinterpret_cast<unsigned int*>(namesChunk.m_data);

		char* namesPtr = reinterpret_cast<char*>(namesCount + 1);

		for (unsigned int i = 0; i < *namesCount; ++i)
		{
			std::string& tmp = names.emplace_back();
			tmp = namesPtr;

			namesPtr += tmp.size() + 1;
		}
	}

	unsigned int geoCount;
	unsigned int objCount;
	unsigned int skelCount;

	{
		data::BinChunk countsChunk;
		countsChunk.Read(reader);

		unsigned int* intData = reinterpret_cast<unsigned int*>(countsChunk.m_data);
		geoCount = intData[0];
		objCount = intData[1];
		skelCount = intData[2];
	}

	for (unsigned int i = 0; i < geoCount; ++i)
	{
		Geometry geo;
		int id;
		geo.Deserialize(reader, id);

		m_geometries[names[id]] = geo;
	}

	for (unsigned int i = 0; i < objCount; ++i)
	{
		Object obj;
		int id;
		obj.Deserialize(reader, id);

		m_objects[names[id]] = obj;
	}

	for (unsigned int i = 0; i < skelCount; ++i)
	{
		Skeleton skel;
		int id;
		skel.Deserialize(reader, id);

		m_skeletons[names[id]] = skel;
	}
}


void collada::Object::Serialize(data::MemoryFileWriter& writer, int id)
{
	std::map<std::string, int> nameIds;

	for (auto it = m_materialOverrides.begin(); it != m_materialOverrides.end(); ++it)
	{
		const std::string& curMaterialOverride = *it;
		nameIds[curMaterialOverride] = 0;
	}
	nameIds[m_geometry] = 0;

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

		namesChunk.Write(writer);
	}

	{
		data::BinChunk idChunk;

		idChunk.m_size = sizeof(int);
		idChunk.m_data = new char[sizeof(int)];

		int* idPtr = reinterpret_cast<int*>(idChunk.m_data);
		*idPtr = id;

		idChunk.Write(writer);
	}

	{
		data::BinChunk transformChunk;
		transformChunk.m_size = _countof(m_transform) * sizeof(float);
		transformChunk.m_data = new char[transformChunk.m_size];

		memcpy(transformChunk.m_data, m_transform, transformChunk.m_size);

		transformChunk.Write(writer);
	}

	{
		data::BinChunk instanceDataChunk;
		instanceDataChunk.m_size = sizeof(m_instanceData);
		instanceDataChunk.m_data = new char[instanceDataChunk.m_size];

		memcpy(instanceDataChunk.m_data, &m_instanceData, instanceDataChunk.m_size);

		instanceDataChunk.Write(writer);
	}

	{
		data::BinChunk geoAndMaterialsChunk;

		unsigned int size = sizeof(unsigned int);
		size += sizeof(int);
		size += m_materialOverrides.size() * sizeof(int);

		geoAndMaterialsChunk.m_size = size;
		geoAndMaterialsChunk.m_data = new char[geoAndMaterialsChunk.m_size];

		unsigned int* cnt = reinterpret_cast<unsigned int*>(geoAndMaterialsChunk.m_data);

		*cnt = m_materialOverrides.size();

		int* geometryPtr = reinterpret_cast<int*>(cnt + 1);
		int* overridesPtr = geometryPtr + 1;

		*geometryPtr = nameIds[m_geometry];
		for (auto it = m_materialOverrides.begin(); it != m_materialOverrides.end(); ++it)
		{
			*overridesPtr = nameIds[*it];
			++overridesPtr;
		}

		geoAndMaterialsChunk.Write(writer);
	}
}

void collada::Object::Deserialize(data::MemoryFileReader& reader, int& id)
{
	std::vector<std::string> names;

	{
		data::BinChunk namesChunk;
		namesChunk.Read(reader);


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
		data::BinChunk idChunk;
		idChunk.Read(reader);

		id = *reinterpret_cast<int*>(idChunk.m_data);
	}

	{
		data::BinChunk transformChunk;
		transformChunk.Read(reader);

		memcpy(m_transform, transformChunk.m_data, _countof(m_transform) * sizeof(float));
	}

	{
		data::BinChunk instanceDataChunk;
		instanceDataChunk.Read(reader);

		m_instanceData = *reinterpret_cast<GeometryInstanceData*>(instanceDataChunk.m_data);
	}

	{
		data::BinChunk geoAndMaterialsChunk;
		geoAndMaterialsChunk.Read(reader);

		unsigned int* numOverrides = reinterpret_cast<unsigned int*>(geoAndMaterialsChunk.m_data);
		int* geo = reinterpret_cast<int*>(numOverrides + 1);
		int* overrides = geo + 1;

		m_geometry = names[*geo];

		for (int i = 0; i < *numOverrides; ++i)
		{
			m_materialOverrides.push_back(names[overrides[i]]);
		}
	}
}

int collada::Matrix::GetIndex(int row, int column)
{
	int index = 4 * row + column;
	return index;
}

float collada::Matrix::GetCoef(int row, int column) const
{
	int index = GetIndex(row, column);

	return m_coefs[index];
}

collada::Matrix collada::Matrix::Transpose() const
{
	Matrix res;

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			res.m_coefs[GetIndex(j, i)] = GetCoef(i, j);
		}
	}

	return res;
}

collada::Matrix collada::Matrix::Multiply(const collada::Matrix& m1, const collada::Matrix& m2)
{
	Matrix res;

	for (int row = 0; row < 4; ++row)
	{
		for (int col = 0; col < 4; ++col)
		{
			float c = 0;
			for (int k = 0; k < 4; ++k)
			{
				c += m1.GetCoef(row, k) * m2.GetCoef(k, col);
			}

			res.m_coefs[Matrix::GetIndex(row, col)] = c;
		}
	}

	return res;
}

const collada::Matrix& collada::Matrix::Zero()
{
	return m_zero;
}

const collada::Matrix& collada::Matrix::One()
{
	return m_one;
}

collada::Vector4 collada::Matrix::TransformPoint(const Vector4& vector) const
{
	Vector4 res;
	for (int i = 0; i < 4; ++i)
	{
		float& curCoef = res.m_values[i];

		for (int j = 0; j < 4; ++j)
		{
			curCoef += vector.m_values[j] * GetCoef(j, i);
		}
	}

	return res;
}

collada::Vector4 collada::Vector4::ConjugateQuat() const
{
	Vector4 res = *this;
	res.m_values[1] = -res.m_values[1];
	res.m_values[2] = -res.m_values[2];
	res.m_values[3] = -res.m_values[3];

	return res;
}

collada::Vector4 collada::Vector4::MultiplyQuat(const collada::Vector4& q1, const collada::Vector4& q2)
{
	Vector4 res;
	res.m_values[0] =
		  q1.m_values[0] * q2.m_values[0]
		- q1.m_values[1] * q2.m_values[1]
		- q1.m_values[2] * q2.m_values[2]
		- q1.m_values[3] * q2.m_values[3];

	res.m_values[1] =
		  q1.m_values[0] * q2.m_values[1]
		+ q1.m_values[1] * q2.m_values[0]
		+ q1.m_values[2] * q2.m_values[3]
		- q1.m_values[3] * q2.m_values[2];

	res.m_values[2] =
		  q1.m_values[0] * q2.m_values[2]
		- q1.m_values[1] * q2.m_values[3]
		+ q1.m_values[2] * q2.m_values[0]
		+ q1.m_values[3] * q2.m_values[1];

	res.m_values[3] =
		  q1.m_values[0] * q2.m_values[3]
		+ q1.m_values[1] * q2.m_values[2]
		- q1.m_values[2] * q2.m_values[1]
		+ q1.m_values[3] * q2.m_values[0];

	return res;
}

collada::Vector3 collada::Vector4::RotateVector(const Vector3& v) const
{
	Vector4 tmp = { 0, v.m_values[0], v.m_values[1], v.m_values[2] };
	tmp = MultiplyQuat(ConjugateQuat(), MultiplyQuat(tmp, *this));

	int yzw[3] = {1,2,3};
	return tmp.GetComponents(yzw);
}

collada::Vector3 collada::Vector4::GetComponents(const int indices[3]) const
{
	Vector3 res = { m_values[indices[0]], m_values[indices[1]], m_values[indices[2]] };
	return res;
}

collada::Vector3 collada::Vector3::Cross(const Vector3& v1, const Vector3& v2)
{
	Vector3 res;
	res.m_values[0] = + v1.m_values[1] * v2.m_values[2] - v1.m_values[2] * v2.m_values[1];
	res.m_values[1] = - v1.m_values[0] * v2.m_values[2] + v1.m_values[2] * v2.m_values[0];
	res.m_values[2] = + v1.m_values[0] * v2.m_values[1] - v1.m_values[1] * v2.m_values[0];

	return res;
}

float collada::Vector3::Dot(const Vector3& v1, const Vector3& v2)
{
	float res = v1.m_values[0] * v2.m_values[0] + v1.m_values[1] * v2.m_values[1] + v1.m_values[2] * v2.m_values[2];

	return res;
}

collada::Vector3 collada::Vector3::Normalize() const
{
	float d = Dot(*this, *this);
	if (d < 0.0000001)
	{
		return Vector3{};
	}

	d = sqrt(d);
	Vector3 res = *this;
	res.m_values[0] /= d;
	res.m_values[1] /= d;
	res.m_values[2] /= d;

	return res;
}

collada::Vector3 collada::Vector3::operator*(float t) const
{
	Vector3 res = *this;
	res.m_values[0] = t * res.m_values[0];
	res.m_values[1] = t * res.m_values[1];
	res.m_values[2] = t * res.m_values[2];

	return res;
}

collada::Vector3 collada::Vector3::operator+(const Vector3& other) const
{
	Vector3 res = {};
	res.m_values[0] = m_values[0] + other.m_values[0];
	res.m_values[1] = m_values[1] + other.m_values[1];
	res.m_values[2] = m_values[2] + other.m_values[2];

	return res;
}

collada::Transform collada::Matrix::ToTransform() const
{
	Transform res;

	Vector4 origin = { {0, 0, 0, 1} };

	int xyz[3] = { 0,1,2 };
	origin = TransformPoint(origin);
	res.m_offset = origin.GetComponents(xyz);

	Vector4 x = { {1, 0, 0, 1} };
	Vector4 y = { {0, 1, 0, 1} };
	Vector4 z = { {0, 0, 1, 1} };

	x = TransformPoint(x);
	y = TransformPoint(y);
	z = TransformPoint(z);

	Vector3 X = x.GetComponents(xyz) + origin.GetComponents(xyz) * (-1);
	Vector3 Y = y.GetComponents(xyz) + origin.GetComponents(xyz) * (-1);
	Vector3 Z = z.GetComponents(xyz) + origin.GetComponents(xyz) * (-1);

	float dX = Vector3::Dot(X, X);
	float dY = Vector3::Dot(Y, Y);
	float dZ = Vector3::Dot(Z, Z);

	dX = sqrt(dX);
	dY = sqrt(dY);
	dZ = sqrt(dZ);

	res.m_scale = { dX, dY, dZ };

	X = X.Normalize();
	Y = Y.Normalize();
	Z = Z.Normalize();

	Vector4 rot = { 1, 0, 0, 0 };

	if (Z.m_values[2] < 1)
	{
		float alt = acos(Z.m_values[2]) / 2;
		rot = { cos(alt), 0, -sin(alt), 0 };

		Vector2 azmDir = { Z.m_values[0], Z.m_values[1] };
		float d = azmDir.m_values[0] * azmDir.m_values[0] + azmDir.m_values[1] * azmDir.m_values[1];
		d = sqrt(d);

		azmDir.m_values[0] /= d;
		azmDir.m_values[1] /= d;

		float azmCos = azmDir.m_values[0];
		azmCos = azmCos < -1 ? -1 : azmCos;
		azmCos = azmCos > 1 ? 1 : azmCos;

		float azm = acos(azmCos);
		if (azmDir.m_values[1] < 0)
		{
			azm *= -1;
		}
		azm /= 2;

		Vector4 r2 = { cos(azm), 0, 0, -sin(azm) };
		rot = Vector4::MultiplyQuat(rot, r2);
	}

	Vector3 X0 = rot.RotateVector(Vector3({1, 0, 0}));

	float cosAngle = Vector3::Dot(X, X0);
	cosAngle = cosAngle < -1 ? -1 : cosAngle;
	cosAngle = cosAngle > 1 ? 1 : cosAngle;

	float angle = acos(cosAngle);

	{
		Vector3 tmp = Vector3::Cross(X0, X);
		if (Vector3::Dot(tmp, Z) < 0)
		{
			angle *= -1;
		}
	}

	angle /= 2;
	
	{
		Vector4 tmp = { cos(angle), -sin(angle) * Z.m_values[0], -sin(angle) * Z.m_values[1], -sin(angle) * Z.m_values[2] };
		rot = Vector4::MultiplyQuat(rot, tmp);
	}

	res.m_rotation = rot;

	{
		Vector3 Y0 = rot.RotateVector(Vector3 { 0, 1, 0 });
		if (Vector3::Dot(Y0, Y) < 0)
		{
			res.m_scale.m_values[1] *= -1;
		}
	}

	return res;
}

collada::Vector3 collada::Transform::TransformPoint(const Vector3& p) const
{
	Vector3 res =
	{
		p.m_values[0] * m_scale.m_values[0],
		p.m_values[1] * m_scale.m_values[1],
		p.m_values[2] * m_scale.m_values[2]
	};

	res = m_rotation.RotateVector(res);
	res = res + m_offset;

	return res;
}

collada::Matrix collada::Transform::ToMatrix() const
{
	Matrix res = Matrix::One();

	Vector3 X = m_rotation.RotateVector(Vector3{ 1, 0, 0 });
	Vector3 Y = m_rotation.RotateVector(Vector3{ 0, 1, 0 });
	Vector3 Z = m_rotation.RotateVector(Vector3{ 0, 0, 1 });

	X = X * m_scale.m_values[0];
	Y = Y * m_scale.m_values[1];
	Z = Z * m_scale.m_values[2];

	res.m_coefs[res.GetIndex(0, 0)] = X.m_values[0];
	res.m_coefs[res.GetIndex(1, 0)] = X.m_values[1];
	res.m_coefs[res.GetIndex(2, 0)] = X.m_values[2];

	res.m_coefs[res.GetIndex(0, 1)] = Y.m_values[0];
	res.m_coefs[res.GetIndex(1, 1)] = Y.m_values[1];
	res.m_coefs[res.GetIndex(2, 1)] = Y.m_values[2];

	res.m_coefs[res.GetIndex(0, 2)] = Z.m_values[0];
	res.m_coefs[res.GetIndex(1, 2)] = Z.m_values[1];
	res.m_coefs[res.GetIndex(2, 2)] = Z.m_values[2];

	res.m_coefs[res.GetIndex(0, 3)] = m_offset.m_values[0];
	res.m_coefs[res.GetIndex(1, 3)] = m_offset.m_values[1];
	res.m_coefs[res.GetIndex(2, 3)] = m_offset.m_values[2];

	res = res.Transpose();
	return res;
}

collada::Transform collada::Transform::Lerp(const Transform& t1, const Transform& t2, float coef)
{
	Transform res;
	res.m_offset = t1.m_offset * (1 - coef) + t2.m_offset * coef;
	res.m_scale = t1.m_scale * (1 - coef) + t2.m_scale * coef;


	int yzw[3] = { 1, 2, 3 };
	Vector4 rot = Vector4::MultiplyQuat(t1.m_rotation.ConjugateQuat(), t2.m_rotation);

	float cosAngle = rot.m_values[0];
	cosAngle = cosAngle < -1 ? -1 : cosAngle;
	cosAngle = cosAngle >  1 ?  1 : cosAngle;

	float angle = acos(cosAngle);
	Vector3 pole = rot.GetComponents(yzw);
	pole = pole.Normalize();

	if (sin(angle) > 0)
	{
		pole = pole * (-1);
	}

	if (angle > M_PI / 2)
	{
		angle -= M_PI;
	}

	angle *= coef;

	rot = Vector4{cos(angle), -sin(angle) * pole.m_values[0], -sin(angle) * pole.m_values[1], -sin(angle) * pole.m_values[2]};

	res.m_rotation = Vector4::MultiplyQuat(t1.m_rotation, rot);

	return res;
}

namespace
{
	struct MatrixWithId
	{
		int m_id = -1;
		collada::Matrix m_matrix;
	};

	struct WeightWithId
	{
		int m_jointId = -1;
		float m_weight = 0;
	};
}

void collada::Skeleton::Serialize(data::MemoryFileWriter& writer, int id)
{
	std::map<std::string, int> nameIds;

	for (auto it = m_joints.begin(); it != m_joints.end(); ++it)
	{
		nameIds[*it] = 0;
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

		namesChunk.Write(writer);
	}

	{
		data::BinChunk idChunk;

		idChunk.m_size = sizeof(int);
		idChunk.m_data = new char[sizeof(int)];

		int* idPtr = reinterpret_cast<int*>(idChunk.m_data);
		*idPtr = id;

		idChunk.Write(writer);
	}

	{
		data::BinChunk jointsChunk;

		jointsChunk.m_size = sizeof(unsigned int);
		jointsChunk.m_size += m_joints.size() * sizeof(unsigned int);

		jointsChunk.m_data = new char[jointsChunk.m_size];

		unsigned int* numJoints = reinterpret_cast<unsigned int*>(jointsChunk.m_data);
		*numJoints = m_joints.size();

		unsigned int* jointNameId = numJoints + 1;

		for (auto it = m_joints.begin(); it != m_joints.end(); ++it)
		{
			*jointNameId++ = nameIds[*it];
		}

		jointsChunk.Write(writer);
	}

	{
		data::BinChunk jointParentsChunk;

		jointParentsChunk.m_size = m_jointsParents.size() * sizeof(int);
		jointParentsChunk.m_data = new char[jointParentsChunk.m_size];

		int* parentsArray = reinterpret_cast<int*>(jointParentsChunk.m_data);

		for (auto it = m_jointsParents.begin(); it != m_jointsParents.end(); ++it)
		{
			*parentsArray++ = *it;
		}

		jointParentsChunk.Write(writer);
	}

	{
		data::BinChunk jointParentsChunk;

		jointParentsChunk.m_size = m_jointsParents.size() * sizeof(Matrix);
		jointParentsChunk.m_data = new char[jointParentsChunk.m_size];

		Matrix* transformsArray = reinterpret_cast<Matrix*>(jointParentsChunk.m_data);

		for (auto it = m_jointTransforms.begin(); it != m_jointTransforms.end(); ++it)
		{
			*transformsArray++ = *it;
		}

		jointParentsChunk.Write(writer);
	}

	{
		data::BinChunk bindShapeMatrixChunk;
		bindShapeMatrixChunk.m_size = sizeof(Matrix);
		bindShapeMatrixChunk.m_data = new char[bindShapeMatrixChunk.m_size];

		Matrix* curData = reinterpret_cast<Matrix*>(bindShapeMatrixChunk.m_data);
		*curData = m_bindShapeMatrix;

		bindShapeMatrixChunk.Write(writer);
	}

	{
		data::BinChunk invertBindMatricesChunk;
		invertBindMatricesChunk.m_size = m_invertBindMatrices.size() * sizeof(MatrixWithId);
		invertBindMatricesChunk.m_data = new char[invertBindMatricesChunk.m_size];

		MatrixWithId* curData = reinterpret_cast<MatrixWithId*>(invertBindMatricesChunk.m_data);
		for (auto it = m_invertBindMatrices.begin(); it != m_invertBindMatrices.end(); ++it)
		{
			curData->m_id = nameIds[it->first];
			curData->m_matrix = it->second;
			++curData;
		}

		invertBindMatricesChunk.Write(writer);
	}

	{
		unsigned int allWeights = 0;
		{
			data::BinChunk weightsCountsChunk;
			weightsCountsChunk.m_size = m_weights.size() * sizeof(unsigned int);
			weightsCountsChunk.m_data = new char[weightsCountsChunk.m_size];

			unsigned int* countsData = reinterpret_cast<unsigned int*>(weightsCountsChunk.m_data);
			for (auto it = m_weights.begin(); it != m_weights.end(); ++it)
			{
				std::list<VertexWeight>& cur = *it;
				*(countsData++) = cur.size();
				allWeights += cur.size();
			}

			weightsCountsChunk.Write(writer);
		}

		{
			struct WeightWithId
			{
				int m_jointId = -1;
				float m_weight = 0;
			};

			data::BinChunk weightsChunk;
			weightsChunk.m_size = allWeights * sizeof(WeightWithId);
			weightsChunk.m_data = new char[weightsChunk.m_size];

			WeightWithId* weightsData = reinterpret_cast<WeightWithId*>(weightsChunk.m_data);
			for (auto it = m_weights.begin(); it != m_weights.end(); ++it)
			{
				std::list<VertexWeight>& cur = *it;
				for (auto weightIt = cur.begin(); weightIt != cur.end(); ++weightIt)
				{
					VertexWeight& curWeight = *weightIt;
					weightsData->m_jointId = nameIds[curWeight.m_joint];
					weightsData->m_weight = curWeight.m_weight;

					++weightsData;
				}
			}
			weightsChunk.Write(writer);
		}
	}
}

void collada::Skeleton::Deserialize(data::MemoryFileReader& reader, int& id)
{
	std::vector<std::string> names;

	{
		data::BinChunk namesChunk;
		namesChunk.Read(reader);


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
		data::BinChunk idChunk;
		idChunk.Read(reader);

		id = *reinterpret_cast<int*>(idChunk.m_data);
	}

	{
		data::BinChunk jointsChunk;
		jointsChunk.Read(reader);

		unsigned int* numJoints = reinterpret_cast<unsigned int*>(jointsChunk.m_data);
		unsigned int* jointIds = numJoints + 1;

		for (unsigned int i = 0; i < *numJoints; ++i)
		{
			m_joints.push_back(names[jointIds[i]]);
		}
	}

	{
		data::BinChunk jointParentsChunk;
		jointParentsChunk.Read(reader);

		int count = jointParentsChunk.m_size / sizeof(int);
		int* data = reinterpret_cast<int*>(jointParentsChunk.m_data);

		for (int i = 0; i < count; ++i)
		{
			m_jointsParents.push_back(data[i]);
		}
	}

	{
		data::BinChunk jointTransformsChunk;
		jointTransformsChunk.Read(reader);

		int count = jointTransformsChunk.m_size / sizeof(Matrix);

		Matrix* data = reinterpret_cast<Matrix*>(jointTransformsChunk.m_data);
		for (int i = 0; i < count; ++i)
		{
			m_jointTransforms.push_back(data[i]);
		}
	}

	{
		data::BinChunk bindShapeMatrixChunk;
		bindShapeMatrixChunk.Read(reader);

		m_bindShapeMatrix = *reinterpret_cast<Matrix*>(bindShapeMatrixChunk.m_data);
	}

	{
		data::BinChunk invertBindMatricesChunk;
		invertBindMatricesChunk.Read(reader);

		int numElements = invertBindMatricesChunk.m_size / sizeof(MatrixWithId);

		MatrixWithId* matrixData = reinterpret_cast<MatrixWithId*>(invertBindMatricesChunk.m_data);
		for (int i = 0; i < numElements; ++i)
		{
			m_invertBindMatrices[names[matrixData->m_id]] = matrixData->m_matrix;
			++matrixData;
		}
	}

	{
		std::list<unsigned int> numWeights;
		{
			data::BinChunk weightsCountsChunk;
			weightsCountsChunk.Read(reader);

			int count = weightsCountsChunk.m_size / sizeof(unsigned int);

			unsigned int* countData = reinterpret_cast<unsigned int*>(weightsCountsChunk.m_data);
			for (int i = 0; i < count; ++i)
			{
				numWeights.push_back(*countData++);
			}
		}

		{
			data::BinChunk weightsChunk;
			weightsChunk.Read(reader);

			WeightWithId* weightsData = reinterpret_cast<WeightWithId*>(weightsChunk.m_data);
			for (auto it = numWeights.begin(); it != numWeights.end(); ++it)
			{
				unsigned int curCount = *it;

				std::list<VertexWeight> weightList;
				for (unsigned int i = 0; i < curCount; ++i)
				{
					VertexWeight& vw = weightList.emplace_back();
					vw.m_joint = names[weightsData->m_jointId];
					vw.m_weight = weightsData->m_weight;

					++weightsData;
				}

				m_weights.push_back(weightList);
			}
		}
	}
}

void collada::KeyFrame::Serialize(data::MemoryFileWriter& writer)
{
	data::BinChunk chunk;
	chunk.m_size = 0;

	int stringSize = m_interpolation.size() + 1;
	chunk.m_size += stringSize * sizeof(char);
	chunk.m_size += sizeof(float);
	chunk.m_size += sizeof(Matrix);

	chunk.m_data = new char[chunk.m_size];
	char* stringData = chunk.m_data;
	memcpy(stringData, m_interpolation.c_str(), stringSize);

	float* timeData = reinterpret_cast<float*>(stringData + stringSize);
	*timeData = m_time;

	Matrix* transformData = reinterpret_cast<Matrix*>(timeData + 1);
	*transformData = m_transform;

	chunk.Write(writer);
}

void collada::KeyFrame::Deserialize(data::MemoryFileReader& reader)
{
	data::BinChunk chunk;
	chunk.Read(reader);

	m_interpolation = chunk.m_data;

	float* timeData = reinterpret_cast<float*>(chunk.m_data + m_interpolation.size() + 1);
	m_time = *timeData;

	Matrix* transformData = reinterpret_cast<Matrix*>(timeData + 1);
	m_transform = *transformData;
}

void collada::AnimChannel::Serialize(data::MemoryFileWriter& writer)
{
	{
		data::BinChunk sizeChunk;
		sizeChunk.m_size = 0;

		int stringSize = m_boneName.size() + 1;
		sizeChunk.m_size += stringSize * sizeof(char);
		sizeChunk.m_size += sizeof(unsigned int);

		sizeChunk.m_data = new char[sizeChunk.m_size];
		char* stringData = sizeChunk.m_data;
		memcpy(stringData, m_boneName.c_str(), stringSize);

		unsigned int* numKeyframesData = reinterpret_cast<unsigned int*>(stringData + stringSize);
		*numKeyframesData = m_keyFrames.size();

		sizeChunk.Write(writer);
	}

	for (auto it = m_keyFrames.begin(); it != m_keyFrames.end(); ++it)
	{
		(*it).Serialize(writer);
	}
}

void collada::AnimChannel::Deserialize(data::MemoryFileReader& reader)
{
	data::BinChunk sizeChunk;
	sizeChunk.Read(reader);

	m_boneName = sizeChunk.m_data;
	unsigned int* numKeyframes = reinterpret_cast<unsigned int*>(sizeChunk.m_data + m_boneName.size() + 1);
	m_keyFrames = std::vector<KeyFrame>(*numKeyframes);

	for (int i = 0; i < *numKeyframes; ++i)
	{
		m_keyFrames[i].Deserialize(reader);
	}
}

void collada::Animation::Serialize(data::MemoryFileWriter& writer)
{
	std::map<std::string, int> nameIds;

	for (auto it = m_bones.begin(); it != m_bones.end(); ++it)
	{
		nameIds[*it] = 0;
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

		namesChunk.Write(writer);
	}

	{
		data::BinChunk bonesChunk;
		bonesChunk.m_size = m_bones.size() * sizeof(int);
		bonesChunk.m_data = new char[bonesChunk.m_size];

		int* boneIDs = reinterpret_cast<int*>(bonesChunk.m_data);
		for (auto it = m_bones.begin(); it != m_bones.end(); ++it)
		{
			*boneIDs++ = nameIds[*it];
		}

		bonesChunk.Write(writer);
	}

	{
		data::BinChunk bonesParentsChunk;
		bonesParentsChunk.m_size = m_boneParents.size() * sizeof(int);
		bonesParentsChunk.m_data = new char[bonesParentsChunk.m_size];

		int* boneParents = reinterpret_cast<int*>(bonesParentsChunk.m_data);
		for (auto it = m_boneParents.begin(); it != m_boneParents.end(); ++it)
		{
			*boneParents++ = *it;
		}

		bonesParentsChunk.Write(writer);
	}

	{
		data::BinChunk numChannelsChunk;
		numChannelsChunk.m_size = sizeof(unsigned int);
		numChannelsChunk.m_data = new char[numChannelsChunk.m_size];

		unsigned int* numChannelsData = reinterpret_cast<unsigned int*>(numChannelsChunk.m_data);
		*numChannelsData = m_channels.size();

		numChannelsChunk.Write(writer);
	}

	for (auto it = m_channels.begin(); it != m_channels.end(); ++it)
	{
		it->second.Serialize(writer);
	}
}

void collada::Animation::Deserialize(data::MemoryFileReader& reader)
{
	std::vector<std::string> names;

	{
		data::BinChunk namesChunk;
		namesChunk.Read(reader);

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
		data::BinChunk bonesChunk;
		bonesChunk.Read(reader);
		
		int size = bonesChunk.m_size / sizeof(int);

		int* boneIDs = reinterpret_cast<int*>(bonesChunk.m_data);
		for (int i = 0; i < size; ++i)
		{
			m_bones.push_back(names[boneIDs[i]]);
		}
	}

	{
		data::BinChunk bonesParentsChunk;
		bonesParentsChunk.Read(reader);

		int size = bonesParentsChunk.m_size / sizeof(int);

		int* boneIDs = reinterpret_cast<int*>(bonesParentsChunk.m_data);
		for (int i = 0; i < size; ++i)
		{
			m_boneParents.push_back(boneIDs[i]);
		}
	}

	unsigned int numChannels = -1;
	{
		data::BinChunk numChannelsChunk;
		numChannelsChunk.Read(reader);
		unsigned int* numChannelsData = reinterpret_cast<unsigned int*>(numChannelsChunk.m_data);
		numChannels = *numChannelsData;
	}

	for (unsigned int i = 0; i < numChannels; ++i)
	{
		AnimChannel tmp;
		tmp.Deserialize(reader);

		m_channels[tmp.m_boneName] = tmp;
	}
}