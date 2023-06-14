#include "Collada.h"

#include "ColladaEntities.h"

#include <queue>
#include <functional>
#include <sstream>
#include <vector>
#include <DirectXMath.h>

namespace
{
	using namespace collada;
	using namespace xml_reader;

	const Node* FindChildTagByName(const std::string& name, const Node* rootNode)
	{
		std::list<const Node*> found;
		FindChildNodes(rootNode, [name](const Node* node) {
			return node->m_tagName == name;
		},
		found);

		if (found.size() == 0) {
			return nullptr;
		}

		return *found.begin();
	}

	void FindChildTagsByName(
		const std::string& name,
		const Node* rootNode,
		std::list<const Node*>& found)
	{
		FindChildNodes(rootNode, [name](const Node* node) {
			return node->m_tagName == name;
		},
		found);
	}

	const Node* FindChildTagByID(
		const std::string& id, const Node* rootNode)
	{
		std::list<const Node*> found;
		FindChildNodes(rootNode, [id](const Node* node) {
			std::map<std::string, std::string>::const_iterator it = node->m_tagProps.find("id");
			if (it == node->m_tagProps.end()) {
				return false;
			}
			return it->second == id;
		},
		found);

		if (found.size() == 0) {
			return nullptr;
		}

		return *found.begin();
	}

	bool ReadMaterialTriangles(const Node* triangles, const Node* geometryNode, Geometry& geometry);

	bool ReadGeometry(const std::string& id, const Node* geometry, bool invertAxis, Scene& scene)
	{
		if (scene.m_geometries.find(id) != scene.m_geometries.end()) {
			return true;
		}

		scene.m_geometries.insert(std::pair<std::string, Geometry>(id, Geometry()));
		Geometry& object = scene.m_geometries[id];

		std::list<const Node*> trianglesTags;
		FindChildTagsByName("triangles", geometry, trianglesTags);

		int firstFreeIndex = 0;
		for (std::list<const Node*>::const_iterator it = trianglesTags.begin();
			it != trianglesTags.end(); ++it) {
			const Node* trianglesTag = *it;

			const std::string& materialName = trianglesTag->m_tagProps.find("material")->second;

			if (!ReadMaterialTriangles(trianglesTag, geometry, object)) {
				return false;
			}

			MaterialIndexRange mir;
			mir.m_name = materialName;
			mir.indexOffset = firstFreeIndex;
			mir.indexCount = object.m_indices.size() - firstFreeIndex;
			object.m_materials.push_back(mir);

			firstFreeIndex = object.m_indices.size();
		}

		std::list<int>::iterator it = object.m_indices.begin();
		while (it != object.m_indices.end()) {
			std::list<int>::iterator second = it;
			++second;
			std::list<int>::iterator third = second;
			++third;

			int tmp = *second;
			*second = *third;
			*third = tmp;

			it = third;
			++it;
		}

		if (invertAxis) {
			for (std::list<Vertex>::iterator it = object.m_vertices.begin(); it != object.m_vertices.end(); ++it) {
				Vertex& cur = *it;
				float tmp = cur.m_position[1];
				cur.m_position[1] = cur.m_position[2];
				cur.m_position[2] = tmp;

				tmp = cur.m_normal[1];
				cur.m_normal[1] = cur.m_normal[2];
				cur.m_normal[2] = tmp;
			}
		}

		return true;
	}

	bool ShouldInvertAxis(const Node* rootDataNode)
	{
		const Node* upAxis = FindChildTagByName("up_axis", rootDataNode);

		if (!upAxis) {
			return false;
		}

		if (upAxis->m_data.size() != 1) {
			return false;
		}

		const scripting::ISymbol* s = *upAxis->m_data.begin();
		if (s->m_name == "Name" && s->m_symbolData.m_string == "Z_UP") {
			return true;
		}

		return false;
	}

	Object* ReadObjectAndGeometryFromNode(const Node* node, const Node* rootDataNode, Scene& scene)
	{
		std::list<const Node*> matrixContainer;
		FindChildNodes(node, [](const Node* x) {
			if (x->m_tagName != "matrix") {
				return false;
			}
			std::map<std::string, std::string>::const_iterator it = x->m_tagProps.find("sid");

			if (it == x->m_tagProps.end()) {
				return false;
			}

			if (it->second != "transform") {
				return false;
			}

			return true;
		}, matrixContainer);

		if (matrixContainer.size() == 0) {
			return nullptr;
		}
		const Node* matrix = *matrixContainer.begin();
		const std::string& objectName = node->m_tagProps.find("id")->second;

		scene.m_objects.insert(std::pair<std::string, Object>(objectName, Object()));
		Object& obj = scene.m_objects[objectName];


		int index = 0;
		for (std::list<scripting::ISymbol*>::const_iterator it = matrix->m_data.begin();
			it != matrix->m_data.end(); ++it) {
			obj.m_transform[index++] = (*it)->m_symbolData.m_number;
		}

		const Node* instanceGeometry = FindChildTagByName("instance_geometry", node);
		if (!instanceGeometry) {
			return nullptr;
		}

		std::string geometryURL;
		{
			std::map<std::string, std::string>::const_iterator urlProp =
				instanceGeometry->m_tagProps.find("url");

			if (urlProp == instanceGeometry->m_tagProps.end()) {
				return nullptr;
			}

			geometryURL = urlProp->second.substr(1);
		}

		obj.m_geometry = geometryURL;

		const Node* geometry = FindChildTagByID(geometryURL, rootDataNode);
		if (!geometry) {
			return nullptr;
		}

		bool invertAxis = ShouldInvertAxis(rootDataNode);
		if (!ReadGeometry(geometryURL, geometry, invertAxis, scene)) {
			return nullptr;
		}
		obj.CalcPositionRotationScale(invertAxis);

		return &obj;
	}

	struct Vector3
	{
		float m_values[3];
	};

	struct Vector2
	{
		float m_values[2];
	};

	bool ReadVectors3D(const Node* verts, std::vector<Vector3>& vectors)
	{
		const Node* arr = FindChildTagByName("float_array", verts);
		const Node* acc = FindChildTagByName("accessor", verts);

		std::list<const Node*> params;
		FindChildTagsByName("param", acc, params);

		int offsets[3] = {-1, -1, -1};

		int index = 0;
		for (std::list<const Node*>::const_iterator it = params.begin();
			it != params.end(); ++it) {
			
			const Node* cur = *it;
			const std::string& name = cur->m_tagProps.find("name")->second;
			if (name == "X") {
				offsets[0] = index;
			}
			if (name == "Y") {
				offsets[1] = index;
			}
			if (name == "Z") {
				offsets[2] = index;
			}
			++index;
		}

		for (int i = 0; i < 3; ++i) {
			if (offsets[i] < 0) {
				return false;
			}
		}

		const std::string strideStr = acc->m_tagProps.find("stride")->second;

		std::stringstream ss(strideStr);
		int stride;
		ss >> stride;

		std::vector<float> data;
		{
			ss = std::stringstream(arr->m_tagProps.find("count")->second);
			int arraySize;
			ss >> arraySize;

			std::list<scripting::ISymbol*>::const_iterator it = arr->m_data.begin();
			for (int i = 0; i < arraySize; ++i)
			{
				data.push_back((*it)->m_symbolData.m_number);
				++it;
			}
		}

		for (std::list<scripting::ISymbol*>::const_iterator it = arr->m_data.begin();
			it != arr->m_data.end(); ++it) {
		}

		for (int i = 0; i < data.size(); i += stride) {
			vectors.push_back(Vector3{ 
				data[i + offsets[0]],
				data[i + offsets[1]],
				data[i + offsets[2]] 
			});
		}

		return true;
	}

	bool ReadUVs(const Node* uvs, std::vector<Vector2>& vectors)
	{
		const Node* arr = FindChildTagByName("float_array", uvs);
		const Node* acc = FindChildTagByName("accessor", uvs);

		std::list<const Node*> params;
		FindChildTagsByName("param", acc, params);

		int offsets[2] = { -1, -1 };

		int index = 0;
		for (std::list<const Node*>::const_iterator it = params.begin();
			it != params.end(); ++it) {

			const Node* cur = *it;
			const std::string& name = cur->m_tagProps.find("name")->second;
			if (name == "S") {
				offsets[0] = index;
			}
			if (name == "T") {
				offsets[1] = index;
			}
			++index;
		}

		for (int i = 0; i < 2; ++i) {
			if (offsets[i] < 0) {
				return false;
			}
		}

		const std::string strideStr = acc->m_tagProps.find("stride")->second;

		std::stringstream ss(strideStr);
		int stride;
		ss >> stride;

		std::vector<float> data;
		for (std::list<scripting::ISymbol*>::const_iterator it = arr->m_data.begin();
			it != arr->m_data.end(); ++it) {
			data.push_back((*it)->m_symbolData.m_number);
		}

		for (int i = 0; i < data.size(); i += stride) {
			vectors.push_back(Vector2{
				data[i + offsets[0]],
				data[i + offsets[1]]
				});
		}

		return true;
	}

	int FindVertexIndex(const Vertex& vertex, const Geometry& geometry)
	{
		int index = 0;
		for (std::list<Vertex>::const_iterator it = geometry.m_vertices.begin();
			it != geometry.m_vertices.end(); ++it) {
			if (vertex.Equals(*it)) {
				return index;
			}
			++index;
		}

		return -1;
	}

	bool ReadMaterialTriangles(const Node* triangles, const Node* geometryNode, Geometry& geometry)
	{
		std::list<const Node*> inputs;
		FindChildTagsByName("input", triangles, inputs);

		int vertexOffset = -1;
		int normalOffset = -1;
		int uvOffset = -1;

		std::vector<Vector3> vertices;
		std::vector<Vector3> normals;
		std::vector<Vector2> uvs;

		for (std::list<const Node*>::const_iterator it = inputs.begin();
			it != inputs.end(); ++it) {
			const Node* cur = *it;

			const std::string& semantic = cur->m_tagProps.find("semantic")->second;
			const std::string& source = cur->m_tagProps.find("source")->second;
			const std::string& offsetStr = cur->m_tagProps.find("offset")->second;
			
			std::stringstream ss(offsetStr);

			if (semantic == "VERTEX") {
				ss >> vertexOffset;

				const Node* vert = FindChildTagByID(source.substr(1), geometryNode);
				if (!vert || vert->m_tagName != "vertices") {
					return false;
				}

				std::list<const Node*> tmp;
				FindChildNodes(vert, [](const Node* node) {
					if (node->m_tagName != "input") {
						return false;
					}
					std::map<std::string, std::string>::const_iterator semanticProp = node->m_tagProps.find("semantic");
					if (semanticProp == node->m_tagProps.end()) {
						return false;
					}
					if (semanticProp->second != "POSITION") {
						return false;
					}

					return true;
				}, tmp);
				
				if (tmp.size() == 0) {
					return false;
				}
				const Node* input = *tmp.begin();

				std::map<std::string, std::string>::const_iterator sourceUrlIt = input->m_tagProps.find("source");
				const std::string& sourceUrl = sourceUrlIt->second;

				const Node* vertsSource = FindChildTagByID(sourceUrl.substr(1), geometryNode);

				if (!ReadVectors3D(vertsSource, vertices)) {
					return false;
				}
			}

			if (semantic == "NORMAL") {
				ss >> normalOffset;

				const Node* norm = FindChildTagByID(source.substr(1), geometryNode);
				if (!norm || norm->m_tagName != "source") {
					return false;
				}

				if (!ReadVectors3D(norm, normals)) {
					return false;
				}
			}

			if (semantic == "TEXCOORD") {
				ss >> uvOffset;

				const Node* uvNode = FindChildTagByID(source.substr(1), geometryNode);
				if (!uvNode || uvNode->m_tagName != "source") {
					return false;
				}

				if (!ReadUVs(uvNode, uvs)) {
					return false;
				}
			}
		}

		if (vertexOffset < 0 || normalOffset < 0 || uvOffset < 0) {
			return false;
		}

		const Node* pTag = FindChildTagByName("p", triangles);

		std::vector<int> indices;
		for (std::list<scripting::ISymbol*>::const_iterator it = pTag->m_data.begin();
			it != pTag->m_data.end(); ++it) {
			indices.push_back(static_cast<int>((*it)->m_symbolData.m_number));
		}
		
		int stride = inputs.size();

		Vertex triangle[3];
		int vertRead = 0;

		for (int i = 0; i < indices.size(); i += stride) {
			Vertex& cur = triangle[vertRead];
			int vertIndex = indices[i + vertexOffset];
			int normalIndex = indices[i + normalOffset];
			int uvIndex = indices[i + uvOffset];

			cur.m_position[0] = vertices[vertIndex].m_values[0];
			cur.m_position[1] = vertices[vertIndex].m_values[1];
			cur.m_position[2] = vertices[vertIndex].m_values[2];

			cur.m_normal[0] = normals[normalIndex].m_values[0];
			cur.m_normal[1] = normals[normalIndex].m_values[1];
			cur.m_normal[2] = normals[normalIndex].m_values[2];

			cur.m_uv[0] = uvs[uvIndex].m_values[0];
			cur.m_uv[1] = uvs[uvIndex].m_values[1];

			++vertRead;

			if (vertRead < 3) {
				continue;
			}

			vertRead = 0;

			for (int j = 0; j < 3; ++j) {
				int vertIndex = FindVertexIndex(triangle[j], geometry);
				if (vertIndex < 0) {
					vertIndex = geometry.m_vertices.size();
					geometry.m_vertices.push_back(triangle[j]);
				}
				geometry.m_indices.push_back(vertIndex);
			}
		}

		return true;
	}

	void ConstructInstanceBuffers(Scene& scene)
	{
		scene.m_instanceBuffers.clear();
		scene.m_objectInstanceMap.clear();

		for (std::map<std::string, Geometry>::const_iterator it = scene.m_geometries.begin();
			it != scene.m_geometries.end(); ++it) {
			scene.m_instanceBuffers.insert(std::pair<std::string, InstanceBuffer>(it->first, InstanceBuffer()));
		}

		for (std::map<std::string, Object>::const_iterator it = scene.m_objects.begin();
			it != scene.m_objects.end(); ++it) {
			InstanceBuffer& cur = scene.m_instanceBuffers[it->second.m_geometry];
			
			cur.m_data.push_back(it->second.m_instanceData);
			scene.m_objectInstanceMap[it->first] = cur.m_data.size() - 1;
		}
	}

	float clamp(float x, float minValue, float maxValue)
	{
		if (x < minValue) {
			x = minValue;
		}

		if (x > maxValue) {
			x = maxValue;
		}

		return x;
	}

	DirectX::XMVECTOR GetQuaternion(const DirectX::XMVECTOR& X, const DirectX::XMVECTOR& Y, const DirectX::XMVECTOR& Z)
	{
		using namespace DirectX;

		const float eps = 0.00000001f;

		const DirectX::XMVECTOR& X0 = XMVectorSet(1, 0, 0, 0);
		const DirectX::XMVECTOR& Y0 = XMVectorSet(0, 1, 0, 0);
		const DirectX::XMVECTOR& Z0 = XMVectorSet(0, 0, 1, 0);

		XMVECTOR pole1 = XMVector3Cross(X0, X);
		if(XMVectorGetX(XMVector3Dot(pole1, pole1)) < eps)
		{
			pole1 = Y0;
		}
		pole1 = XMVector3Normalize(pole1);

		float cosAngle1 = XMVectorGetX(XMVector3Dot(X0, X));
		cosAngle1 = clamp(cosAngle1, -1, 1);
		float angle1 = acos(cosAngle1);

		XMVECTOR q1 = -sin(angle1 / 2) * pole1;
		q1 = XMVectorSetW(q1, cos(angle1 / 2));

		XMVECTOR Y1 = XMQuaternionMultiply(q1, XMQuaternionMultiply(Y0, XMQuaternionConjugate(q1)));
		XMVECTOR pole2 = XMVector3Cross(Y1, Y);

		if (XMVectorGetX(XMVector3Dot(pole2, pole2)) < eps)
		{
			pole2 = X;
		}
		pole2 = XMVector3Normalize(pole2);

		float cosAngle2 = XMVectorGetX(XMVector3Dot(Y1, Y));
		cosAngle2 = clamp(cosAngle2, -1, 1);
		float angle2 = acos(cosAngle2);

		XMVECTOR q2 = -sin(angle2 / 2) * pole2;
		q2 = XMVectorSetW(q2, cos(angle2 / 2));

		XMVECTOR res = XMQuaternionMultiply(q2, q1);
		return res;
	}

	struct MaterialInfo
	{
		std::string m_id;
		collada::ColladaMaterial m_material;
	};

	void ReadMaterials(const std::list<Node*>& nodes, std::list<MaterialInfo>& materials)
	{
		const Node* libraryMaterialsTag = nullptr;
		const Node* libraryEffectsTag = nullptr;

		for (std::list<Node*>::const_iterator it = nodes.begin(); it != nodes.end(); ++it) {
			libraryMaterialsTag = FindChildTagByName("library_materials", *it);
			if (libraryMaterialsTag) {
				break;
			}
		}
		if (!libraryMaterialsTag)
		{
			return;
		}

		for (std::list<Node*>::const_iterator it = nodes.begin(); it != nodes.end(); ++it) {
			libraryEffectsTag = FindChildTagByName("library_effects", *it);
			if (libraryEffectsTag) {
				break;
			}
		}
		if (!libraryEffectsTag)
		{
			return;
		}

		std::map<std::string, const Node*> effectsMap;

		for (auto effectIt = libraryEffectsTag->m_children.begin(); effectIt != libraryEffectsTag->m_children.end(); ++effectIt)
		{
			const Node* curEffect = *effectIt;
			if (curEffect->m_tagName != "effect")
			{
				continue;
			}

			effectsMap[curEffect->m_tagProps.find("id")->second] = curEffect;
		}

		for (auto matIt = libraryMaterialsTag->m_children.begin(); matIt != libraryMaterialsTag->m_children.end(); ++matIt)
		{
			const Node* curMat = *matIt;

			if (curMat->m_tagName != "material")
			{
				continue;
			}
			const Node* instance_effect = FindChildTagByName("instance_effect", curMat);
			std::string url = instance_effect->m_tagProps.find("url")->second;

			url = url.substr(1, url.size() - 1);

			const Node* effect = effectsMap.find(url)->second;
			const Node* diffuse = FindChildTagByName("diffuse", effect);
			const Node* diffuseColor = FindChildTagByName("color", diffuse);

			MaterialInfo matInfo;
			matInfo.m_id = curMat->m_tagProps.find("id")->second;
			matInfo.m_material.m_name = curMat->m_tagProps.find("name")->second;


			auto it = diffuseColor->m_data.begin();
			for (int i = 0; i < 4; ++i)
			{
				matInfo.m_material.m_diffuseColor[i] = (*it)->m_symbolData.m_number;
				++it;
			}

			materials.push_back(matInfo);
		}
	}

	void ReadMaterialsBindings(const Node& objectNode, std::list<std::pair<std::string, std::string>>& bindings)
	{
		const Node* materialBind = FindChildTagByName("bind_material", &objectNode);
		if (!materialBind)
		{
			return;
		}

		std::list<const Node*> found;
		FindChildTagsByName("instance_material", materialBind, found);

		for (auto it = found.begin(); it != found.end(); ++it)
		{
			const Node* instanceMaterial = *it;
			bindings.push_back(std::pair<std::string, std::string>(instanceMaterial->m_tagProps.find("symbol")->second, instanceMaterial->m_tagProps.find("target")->second));
		}
	}
}

bool collada::ConvertToScene(const std::list<Node*>& nodes, collada::Scene& scene)
{
	const Node* sceneTag = nullptr;
	const Node* dataContainerTag = nullptr;

	for (std::list<Node*>::const_iterator it = nodes.begin(); it != nodes.end(); ++it) {
		sceneTag = FindChildTagByName("scene", *it);
		if (sceneTag) {
			dataContainerTag = *it;
			break;
		}
	}

	if (!sceneTag) {
		return false;
	}

	std::list<MaterialInfo> materials;
	ReadMaterials(nodes, materials);

	for (auto it = materials.begin(); it != materials.end(); ++it)
	{
		scene.m_materials[it->m_material.m_name] = it->m_material;
	}

	const Node* instanceVisualScene = FindChildTagByName("instance_visual_scene", sceneTag);

	if (!instanceVisualScene) {
		return false;
	}

	std::string visualSceneURL;
	{
		std::map<std::string, std::string>::const_iterator urlIt = instanceVisualScene->m_tagProps.find("url");
		if (urlIt == instanceVisualScene->m_tagProps.end()) {
			return false;
		}

		visualSceneURL = urlIt->second.substr(1);
	}

	const Node* visualScene = FindChildTagByID(visualSceneURL, dataContainerTag);
	if (!visualScene) {
		return false;
	}

	std::list<const Node*> objectNodes;
	FindChildTagsByName("node", visualScene, objectNodes);

	for (std::list<const Node*>::const_iterator it = objectNodes.begin();
		it != objectNodes.end(); ++it) 
	{
		Object* object = ReadObjectAndGeometryFromNode(*it, dataContainerTag, scene);
		if (!object) {
			return false;
		}

		std::list<std::pair<std::string, std::string>> bindings;
		ReadMaterialsBindings(*(*it), bindings);

		const Geometry& geo = scene.m_geometries.find(object->m_geometry)->second;
		for (auto matIt = geo.m_materials.begin(); matIt != geo.m_materials.end(); ++matIt)
		{
			std::string overrideName = "error";
			for (auto bindIt = bindings.begin(); bindIt != bindings.end(); ++bindIt)
			{
				if (bindIt->first == matIt->m_name)
				{
					overrideName = bindIt->second;
					overrideName = overrideName.substr(1, overrideName.size() - 1);
					break;
				}
			}

			for (auto matInfoIt = materials.begin(); matInfoIt != materials.end(); ++matInfoIt)
			{
				if (matInfoIt->m_id == overrideName)
				{
					overrideName = matInfoIt->m_material.m_name;
					break;
				}
			}

			object->m_materialOverrides.push_back(overrideName);
		}
	}

	ConstructInstanceBuffers(scene);

	return true;
}

bool collada::Vertex::Equals(const Vertex& other) const
{
	static const float EPS = 0.000001;
	for (int i = 0; i < 3; ++i) {
		if (abs(m_position[i] - other.m_position[i]) >= EPS) {
			return false;
		}
	}

	for (int i = 0; i < 3; ++i) {
		if (abs(m_normal[i] - other.m_normal[i]) >= EPS) {
			return false;
		}
	}

	for (int i = 0; i < 2; ++i) {
		if (abs(m_uv[i] - other.m_uv[i]) >= EPS) {
			return false;
		}
	}

	return true;
}

void collada::Object::InvertAxis()
{
	using namespace DirectX;
	float yUp[16] = {
		1,0,0,0,
		0,0,1,0,
		0,1,0,0,
		0,0,0,1
	};

	XMMATRIX yUpMat(yUp);

	XMMATRIX trMat(m_transform);
	
	trMat = yUpMat * trMat * yUpMat;

	m_transform[0] = XMVectorGetX(trMat.r[0]);
	m_transform[1] = XMVectorGetY(trMat.r[0]);
	m_transform[2] = XMVectorGetZ(trMat.r[0]);
	m_transform[3] = XMVectorGetW(trMat.r[0]);

	m_transform[4] = XMVectorGetX(trMat.r[1]);
	m_transform[5] = XMVectorGetY(trMat.r[1]);
	m_transform[6] = XMVectorGetZ(trMat.r[1]);
	m_transform[7] = XMVectorGetW(trMat.r[1]);

	m_transform[8]  = XMVectorGetX(trMat.r[2]);
	m_transform[9]  = XMVectorGetY(trMat.r[2]);
	m_transform[10] = XMVectorGetZ(trMat.r[2]);
	m_transform[11] = XMVectorGetW(trMat.r[2]);

	m_transform[12] = XMVectorGetX(trMat.r[3]);
	m_transform[13] = XMVectorGetY(trMat.r[3]);
	m_transform[14] = XMVectorGetZ(trMat.r[3]);
	m_transform[15] = XMVectorGetW(trMat.r[3]);
}

void collada::Object::CalcPositionRotationScale(bool invertAxis)
{
	using namespace DirectX;

	XMMATRIX trMat(m_transform);
	trMat = XMMatrixTranspose(trMat);

	XMVECTOR zero = XMVectorSet(0, 0, 0, 1);
	XMVECTOR offset = XMVector4Transform(zero, trMat);

	m_instanceData.m_position[0] = XMVectorGetX(offset);
	m_instanceData.m_position[1] = XMVectorGetY(offset);
	m_instanceData.m_position[2] = XMVectorGetZ(offset);

	XMVECTOR X0 = XMVectorSet(1, 0, 0, 1);
	XMVECTOR Y0 = XMVectorSet(0, 1, 0, 1);
	XMVECTOR Z0 = XMVectorSet(0, 0, 1, 1);

	XMVECTOR X = XMVector4Transform(X0, trMat) - offset;
	XMVECTOR Y = XMVector4Transform(Y0, trMat) - offset;
	XMVECTOR Z = XMVector4Transform(Z0, trMat) - offset;

	m_instanceData.m_scale[0] = XMVectorGetX(XMVector3Length(X));
	m_instanceData.m_scale[1] = XMVectorGetX(XMVector3Length(Y));
	m_instanceData.m_scale[2] = XMVectorGetX(XMVector3Length(Z));

	X /= m_instanceData.m_scale[0];
	Y /= m_instanceData.m_scale[1];
	Z /= m_instanceData.m_scale[2];

	XMVECTOR rot = GetQuaternion(X, Y, Z);

	m_instanceData.m_rotation[0] = XMVectorGetW(rot);
	m_instanceData.m_rotation[1] = XMVectorGetX(rot);
	m_instanceData.m_rotation[2] = XMVectorGetY(rot);
	m_instanceData.m_rotation[3] = XMVectorGetZ(rot);

	if (invertAxis)
	{
		{
			float tmp = m_instanceData.m_position[1];
			m_instanceData.m_position[1] = m_instanceData.m_position[2];
			m_instanceData.m_position[2] = tmp;
		}

		{
			float tmp = m_instanceData.m_scale[1];
			m_instanceData.m_scale[1] = m_instanceData.m_scale[2];
			m_instanceData.m_scale[2] = tmp;
		}

		{
			float tmp = m_instanceData.m_rotation[2];
			m_instanceData.m_rotation[2] = m_instanceData.m_rotation[3];
			m_instanceData.m_rotation[3] = tmp;
		}
	}
}
