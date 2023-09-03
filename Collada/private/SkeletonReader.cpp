#include "SkeletonReader.h"

#include "XMLReader.h"

#include "SceneBuilderUtils.h"
#include "ColladaEntities.h"

#include <sstream>

namespace
{
	collada::Matrix ChangeAxisOfMatrix(const collada::Matrix& matrix)
	{
		using namespace collada;


		Matrix changeBasisMatrix = Matrix::Zero();
		changeBasisMatrix.m_coefs[Matrix::GetIndex(0, 0)] = 1;
		changeBasisMatrix.m_coefs[Matrix::GetIndex(1, 2)] = 1;
		changeBasisMatrix.m_coefs[Matrix::GetIndex(2, 1)] = 1;
		changeBasisMatrix.m_coefs[Matrix::GetIndex(3, 3)] = 1;

		Matrix res = Matrix::Multiply(Matrix::Multiply(changeBasisMatrix, matrix), changeBasisMatrix);
		res = res.Transpose();

		return res;
	}

	void ReadJointNamesFromSkinTag(const xml_reader::Node* skinTag, collada::SkeletonReader& skeleton)
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

	void ReadJointInverseBindMatricesFromSkinTag(const xml_reader::Node* skinTag, collada::SkeletonReader& skeleton)
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

	void ReadVertexWeightsArray(const xml_reader::Node* skinTag, std::vector<float>& weights)
	{
		using namespace xml_reader;

		const Node* vertexWeightsTag = FindChildNode(skinTag, [](const Node* n) {
			bool res = n->m_tagName == "vertex_weights";
			return res;
		});

		const Node* inputTag = FindChildNode(vertexWeightsTag, [](const Node* n) {
			if (n->m_tagName != "input")
			{
				return false;
			}

			if (n->m_tagProps.find("semantic")->second != "WEIGHT")
			{
				return false;
			}
			return true;
		});

		std::string sourceTagId = inputTag->m_tagProps.find("source")->second;
		sourceTagId = sourceTagId.substr(1);

		const Node* weightsArraySourceTag = FindChildNode(skinTag, [=](const Node* n) {
			if (n->m_tagName != "source")
			{
				return false;
			}

			if (n->m_tagProps.find("id")->second != sourceTagId)
			{
				return false;
			}

			return true;
		});

		const Node* accessorTag = FindChildNode(weightsArraySourceTag, [](const Node* n) {
			if (n->m_tagName != "accessor")
			{
				return false;
			}

			const Node* weightsArrayTag = FindChildNode(n, [](const Node* paramTag) {
				if (paramTag->m_tagName != "param")
				{
					return false;
				}
				if (paramTag->m_tagProps.find("name")->second != "WEIGHT")
				{
					return false;
				}

				return true;
			});

			if (!weightsArrayTag)
			{
				return false;
			}

			return true;
		});

		const Node* arrayTag = nullptr;
		{
			std::string url = accessorTag->m_tagProps.find("source")->second;
			url = url.substr(1);

			arrayTag = FindChildNode(weightsArraySourceTag, [=](const Node* n) {
				if (n->m_tagName != "float_array")
				{
					return false;
				}

				if (n->m_tagProps.find("id")->second != url)
				{
					return false;
				}

				return true;
			});
		}

		int weightsSize = 0;
		{
			std::stringstream ss(accessorTag->m_tagProps.find("count")->second);
			ss >> weightsSize;
		}

		auto it = arrayTag->m_data.begin();
		for (int i = 0; i < weightsSize; ++i)
		{
			weights.push_back((*it++)->m_symbolData.m_number);
		}
	}

	struct JointWeightPair
	{
		int m_joint;
		int m_weight;
	};
	struct VertexWeights
	{
		std::vector<JointWeightPair> m_weights;
	};

	void ReadVertexJointWeightsFromSkinTag(const xml_reader::Node* skinTag, std::vector<VertexWeights>& weights)
	{
		using namespace xml_reader;

		const Node* vertexWeightsTag = FindChildNode(skinTag, [](const Node* n) {
			bool res = n->m_tagName == "vertex_weights";
			return res;
		});

		const Node* weightInputTag = FindChildNode(vertexWeightsTag, [](const Node* n) {
			if (n->m_tagName != "input")
			{
				return false;
			}

			if (n->m_tagProps.find("semantic")->second != "WEIGHT")
			{
				return false;
			}
			return true;
		});

		const Node* jointInputTag = FindChildNode(vertexWeightsTag, [](const Node* n) {
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

		int weightOffset = 0;
		int jointOffset = 0;
		{
			std::stringstream ss(weightInputTag->m_tagProps.find("offset")->second);
			ss >> weightOffset;

			ss.clear();
			ss.str(jointInputTag->m_tagProps.find("offset")->second);
			ss >> jointOffset;
		}

		const Node* vCount = FindChildNode(vertexWeightsTag, [](const Node* n) {
			bool res = n->m_tagName == "vcount";
			return res;
		});
		const Node* v = FindChildNode(vertexWeightsTag, [](const Node* n) {
			bool res = n->m_tagName == "v";
			return res;
		});

		int dataCount = 0;
		{
			std::stringstream ss(vertexWeightsTag->m_tagProps.find("count")->second);
			ss >> dataCount;
		}

		auto vCountIt = vCount->m_data.begin();
		auto vIt = v->m_data.begin();

		for (int i = 0; i < dataCount; ++i)
		{
			int cur = (*vCountIt++)->m_symbolData.m_number;

			VertexWeights& curWeights = weights.emplace_back();
			for (int j = 0; j < cur; ++j)
			{
				float tmp[2] =
				{
					(*vIt++)->m_symbolData.m_number,
					(*vIt++)->m_symbolData.m_number,
				};

				curWeights.m_weights.push_back(JointWeightPair{ static_cast<int>(tmp[jointOffset]), static_cast<int>(tmp[weightOffset]) });
			}
		}
	}

	void ReadJointsFromSkinTag(const xml_reader::Node* skinTag, collada::SkeletonReader& skeleton)
	{
		ReadJointNamesFromSkinTag(skinTag, skeleton);
		ReadJointInverseBindMatricesFromSkinTag(skinTag, skeleton);

		std::vector<float> weightsArray;
		std::vector<VertexWeights> vertexWeights;
		ReadVertexWeightsArray(skinTag, weightsArray);
		ReadVertexJointWeightsFromSkinTag(skinTag, vertexWeights);

		for (auto it = vertexWeights.begin(); it != vertexWeights.end(); ++it)
		{
			VertexWeights& cur = *it;

			std::map<std::string, float>& weightsPerVertex = skeleton.m_weights.emplace_back();
			for (auto jointIt = cur.m_weights.begin(); jointIt != cur.m_weights.end(); ++jointIt)
			{
				JointWeightPair& pair = *jointIt;
				weightsPerVertex[skeleton.m_joints[pair.m_joint]] = weightsArray[pair.m_weight];
			}
		}
	}

	void ReadJointHierarchy(const xml_reader::Node* node, collada::SkeletonReader& skeletonReader)
	{
		using namespace xml_reader;
		std::vector<const Node*> jointNodes;

		skeletonReader.m_jointsParents.clear();
		skeletonReader.m_jointTransforms.clear();

		for (auto it = skeletonReader.m_joints.begin(); it != skeletonReader.m_joints.end(); ++it)
		{
			skeletonReader.m_jointsParents.push_back(-1);
			const Node* jointNode = FindChildNode(node, [=](const Node* x) {
				auto sidIt = x->m_tagProps.find("sid");
				if (sidIt == x->m_tagProps.end())
				{
					return false;
				}

				if (sidIt->second != *it)
				{
					return false;
				}

				return true;
			});

			jointNodes.push_back(jointNode);
			collada::Matrix& tmp = skeletonReader.m_jointTransforms.emplace_back();
			if (jointNode)
			{
				collada::GetLocalTransformNode(jointNode, tmp);
			}
		}

		for (int i = 0; i < jointNodes.size(); ++i)
		{
			const Node* cur = jointNodes[i];
			const Node* parent = cur->m_parent;

			for (int j = 0; j < jointNodes.size(); ++j)
			{
				if (parent == jointNodes[j])
				{
					skeletonReader.m_jointsParents[i] = j;
					break;
				}
			}
		}
	}
}

bool collada::SkeletonReader::ReadFromNode(const xml_reader::Node* node, const xml_reader::Node* visualSceneNode, const xml_reader::Node* containerNode, std::string& geometryName)
{
	using namespace xml_reader;

	const Node* controllerTag = nullptr;
	for (auto it = node->m_children.begin(); it != node->m_children.end(); ++it)
	{
		Node* cur = *it;
		if (cur->m_tagName == "instance_controller")
		{
			controllerTag = cur;
			break;
		}
	}
	
	if (!controllerTag)
	{
		return false;
	}

	bool invertAxis = ShouldInvertAxis(containerNode);

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

		if (!ReadGeometry(geometrySource, geoNode, invertAxis, m_scene))
		{
			return false;
		}
		geometryName = geometrySource;
	}

	const Node* bindShapeMatrixTag = FindChildNode(skinTag, [=](const Node* n) {
		bool res = n->m_tagName == "bind_shape_matrix";
		return res;
	});

	ReadMatricesFromNode(bindShapeMatrixTag, &m_bindShapeMatrix, 1);

	ReadJointsFromSkinTag(skinTag, *this);
	ReadJointHierarchy(visualSceneNode, *this);

	if (invertAxis)
	{
		m_bindShapeMatrix = ChangeAxisOfMatrix(m_bindShapeMatrix);

		for (auto it = m_invertBindMatrices.begin(); it != m_invertBindMatrices.end(); ++it)
		{
			Matrix& cur = *it;
			cur = ChangeAxisOfMatrix(cur);
		}

		for (auto it = m_jointTransforms.begin(); it != m_jointTransforms.end(); ++it)
		{
			Matrix& cur = *it;
			cur = ChangeAxisOfMatrix(cur);
		}
	}

	return true;
}


collada::SkeletonReader::SkeletonReader(Scene& scene) :
	m_scene(scene)
{
}

void collada::SkeletonReader::ToSkeleton(collada::Skeleton& skeleton)
{
	skeleton.m_joints = m_joints;
	skeleton.m_jointsParents = m_jointsParents;
	skeleton.m_bindShapeMatrix = m_bindShapeMatrix;

	for (int i = 0; i < m_invertBindMatrices.size(); ++i)
	{
		skeleton.m_invertBindMatrices[m_joints[i]] = m_invertBindMatrices[i];
	}

	for (auto weightIt = m_weights.begin(); weightIt != m_weights.end(); ++weightIt)
	{
		std::list<Skeleton::VertexWeight>& weightsSet = skeleton.m_weights.emplace_back();
		std::map<std::string, float>& cur = *weightIt;

		for (auto it = cur.begin(); it != cur.end(); ++it)
		{
			Skeleton::VertexWeight& weight = weightsSet.emplace_back();
			weight.m_joint = it->first;
			weight.m_weight = it->second;
		}
	}
}

void collada::ReadMatricesFromNode(const xml_reader::Node* node, collada::Matrix* matrices, int numMatrices)
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