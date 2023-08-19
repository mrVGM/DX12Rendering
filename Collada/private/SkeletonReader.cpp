#include "SkeletonReader.h"

#include "ColladaEntities.h"
#include "XMLReader.h"

#include "SceneBuilderUtils.h"

#include <sstream>

namespace
{
	void ReadMatricesFromNode(const xml_reader::Node* node, collada::Matrix* matrices, int numMatrices)
	{
		auto it = node->m_data.begin();

		for (int matrixIndex = 0; matrixIndex < numMatrices; ++matrixIndex)
		{
			for (int i = 0; i < 16; ++i)
			{
				matrices[matrixIndex].m_coefs[i] = (*it++)->m_symbolData.m_number;
			}
		}
	}

	void ReadJointNamesFromSkinTag(const xml_reader::Node* skinTag, collada::Skeleton& skeleton)
	{
		using namespace xml_reader;

		const Node* jointsTag = FindChildNode(skinTag, [](const Node* n) {
			bool res = n->m_tagName == "joints";
			return res;
		});

		const Node* jointNamesTag = FindChildNode(skinTag, [](const Node* n) {
			if (n->m_tagName != "input")
			{
				return false;
			}

			if (n->m_tagProps.find("semantic")->second != "JOINT")
			{
				return false;
			}

			return true;
		});

		std::string jointNamesTagSourceName = jointNamesTag->m_tagProps.find("source")->second;
		jointNamesTagSourceName = jointNamesTagSourceName.substr(1);

		const Node* jointNamesSourceTag = FindChildNode(skinTag, [=](const Node* n) {
			if (n->m_tagName != "source")
			{
				return false;
			}

			if (n->m_tagProps.find("id")->second != jointNamesTagSourceName)
			{
				return false;
			}

			return true;
		});

		const Node* namesArray = FindChildNode(jointNamesSourceTag, [=](const Node* n) {
			bool res = n->m_tagName == "Name_array";
			return res;
		});

		for (auto it = namesArray->m_data.begin(); it != namesArray->m_data.end(); ++it)
		{
			skeleton.m_joints.push_back((*it)->m_symbolData.m_string);
		}
	}

	void ReadJointInverseBindMatricesFromSkinTag(const xml_reader::Node* skinTag, collada::Skeleton& skeleton)
	{
		using namespace xml_reader;

		const Node* jointsTag = FindChildNode(skinTag, [](const Node* n) {
			bool res = n->m_tagName == "joints";
			return res;
		});

		const Node* invertBindMatricesTag = FindChildNode(skinTag, [](const Node* n) {
			if (n->m_tagName != "input")
			{
				return false;
			}

			if (n->m_tagProps.find("semantic")->second != "INV_BIND_MATRIX")
			{
				return false;
			}

			return true;
		});

		std::string invertBindMatricesSourceName = invertBindMatricesTag->m_tagProps.find("source")->second;
		invertBindMatricesSourceName = invertBindMatricesSourceName.substr(1);

		const Node* jointInvertBindMatricesSourceTag = FindChildNode(skinTag, [=](const Node* n) {
			if (n->m_tagName != "source")
			{
				return false;
			}

			if (n->m_tagProps.find("id")->second != invertBindMatricesSourceName)
			{
				return false;
			}

			return true;
		});

		const Node* accessorNode = FindChildNode(jointInvertBindMatricesSourceTag, [](const Node* n) {
			if (n->m_tagName != "accessor")
			{
				return false;
			}
			const Node* transformParam = FindChildNode(n, [](const Node* paramNode) {
				if (paramNode->m_tagName != "param")
				{
					return false;
				}
				if (paramNode->m_tagProps.find("name")->second != "TRANSFORM")
				{
					return false;
				}

				return true;
			});

			if (!transformParam)
			{
				return false;
			}

			return true;
		});

		const Node* floatArray = nullptr;
		{
			std::string floatArrayId = accessorNode->m_tagProps.find("source")->second;
			floatArrayId = floatArrayId.substr(1);

			floatArray = FindChildNode(jointInvertBindMatricesSourceTag, [=](const Node* n) {
				if (n->m_tagName != "float_array")
				{
					return false;
				}

				if (n->m_tagProps.find("id")->second != floatArrayId)
				{
					return false;
				}
				return true;
			});
		}

		int matrixCount = 0;
		{
			std::stringstream ss(accessorNode->m_tagProps.find("count")->second);
			ss >> matrixCount;
		}

		collada::Matrix* matrixTmp = new collada::Matrix[matrixCount];

		ReadMatricesFromNode(floatArray, matrixTmp, matrixCount);

		for (int i = 0; i < matrixCount; ++i)
		{
			skeleton.m_invertBindMatrices.push_back(matrixTmp[i]);
		}

		delete[] matrixTmp;
	}

	void ReadJointsFromSkinTag(const xml_reader::Node* skinTag, collada::Skeleton& skeleton)
	{
		ReadJointNamesFromSkinTag(skinTag, skeleton);
		ReadJointInverseBindMatricesFromSkinTag(skinTag, skeleton);
	}
}

bool collada::Skeleton::ReadFromNode(const xml_reader::Node* node, const xml_reader::Node* containerNode)
{
	using namespace xml_reader;

	const Node* controllerTag = FindChildNode(node, [](const Node* n) {
		bool res = n->m_tagName == "instance_controller";
		return res;
	});
	
	if (!controllerTag)
	{
		return false;
	}

	const Node* libraryControllersTag = FindChildNode(containerNode, [](const Node* n) {
		bool res = n->m_tagName == "library_controllers";
		return res;
	});

	const Node* controllerInstanceTag = FindChildNode(libraryControllersTag, [=](const Node* n) {
		if (n->m_tagName != "controller")
		{
			return false;
		}

		std::string url = controllerTag->m_tagProps.find("url")->second;
		url = url.substr(1);

		if (url != n->m_tagProps.find("id")->second)
		{
			return false;
		}

		return true;
	});

	const Node* skinTag = FindChildNode(controllerInstanceTag, [](const Node* n) {
		bool res = n->m_tagName == "skin";
		return res;
	});

	{
		std::string geometrySource = skinTag->m_tagProps.find("source")->second;
		geometrySource = geometrySource.substr(1);

		const Node* geometryLibrary = FindChildNode(containerNode, [](const Node* n) {
			bool res = n->m_tagName == "library_geometries";
			return res;
		});

		const Node* geoNode = FindChildNode(geometryLibrary, [=](const Node* n) {
			if (n->m_tagName != "geometry")
			{
				return false;
			}

			if (n->m_tagProps.find("id")->second != geometrySource)
			{
				return false;
			}

			return true;
		});

		bool invertAxis = ShouldInvertAxis(containerNode);
		ReadGeometry(geometrySource, geoNode, invertAxis, m_scene);
	}

	const Node* bindShapeMatrixTag = FindChildNode(skinTag, [=](const Node* n) {
		bool res = n->m_tagName == "bind_shape_matrix";
		return res;
	});

	Matrix bindShapeMatrix[1];

	ReadMatricesFromNode(bindShapeMatrixTag, bindShapeMatrix, _countof(bindShapeMatrix));

	ReadJointsFromSkinTag(skinTag, *this);

	return true;
}


collada::Skeleton::Skeleton(Scene& scene) :
	m_scene(scene)
{
}