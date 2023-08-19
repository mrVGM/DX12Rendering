#include "SkeletonReader.h"

#include "ColladaEntities.h"
#include "XMLReader.h"

#include "SceneBuilderUtils.h"

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

		ReadGeometry(geometrySource, geoNode, false, m_scene);
	}

	const Node* bindShapeMatrixTag = FindChildNode(skinTag, [=](const Node* n) {
		bool res = n->m_tagName == "bind_shape_matrix";
		return res;
	});

	Matrix bindShapeMatrix[1];

	ReadMatricesFromNode(bindShapeMatrixTag, bindShapeMatrix, _countof(bindShapeMatrix));


	return true;
}


collada::Skeleton::Skeleton(Scene& scene) :
	m_scene(scene)
{
}