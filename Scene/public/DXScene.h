#pragma once

#include "BaseObject.h"

#include "ColladaScene.h"
#include "Job.h"
#include "JobSystem.h"

#include <vector>
#include <string>
#include <map>

#include <DirectXMath.h>

class BaseObjectMeta;

namespace rendering
{
	class DXBuffer;
	class DXMutableBuffer;
	class DXScene : public BaseObject
	{
	private:
		int m_scenesLoaded = 0;
	public:
		struct GeometryResources
		{
			DXBuffer* m_vertexBuffer = nullptr;
			DXBuffer* m_indexBuffer = nullptr;
			DXMutableBuffer* m_instanceBuffer = nullptr;

			DXBuffer* m_skeletalMeshVertexBuffer = nullptr;
			DXBuffer* m_skeletonBuffer = nullptr;
		};

		struct ObjectResources
		{
			DXMutableBuffer* m_skeletonPoseBuffer = nullptr;
		};

		struct SceneResources
		{
			std::map<std::string, GeometryResources> m_geometryResources;
			std::map<std::string, ObjectResources> m_objectResources;
		};

	private:
		void LoadGeometryBuffers(int sceneIndex, const std::string& geometryName, SceneResources& sceneResources, jobs::Job* done);
		void LoadObjectBuffers(int sceneIndex, const std::string& objectName, SceneResources& sceneResources, jobs::Job* done);
		
		void LoadBuffers(int sceneIndex, jobs::Job* done);
	public:
		std::vector<collada::ColladaScene*> m_colladaScenes;
		std::vector<SceneResources> m_sceneResources;

		collada::Animation m_animation;

		DXScene();
		virtual ~DXScene();

		void LoadColladaScene(const std::string& sceneId, jobs::Job* done);
		void LoadAnimation(const std::string& animId);

		void GetSceneBB(DirectX::XMVECTOR& minPoint, DirectX::XMVECTOR& maxPoint);

		int GetScenesCount();
	};
}