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
	public:
		int m_scenesLoaded = 0;
		struct GeometryResources
		{
			DXBuffer* m_vertexBuffer = nullptr;
			DXBuffer* m_indexBuffer = nullptr;
			DXMutableBuffer* m_instanceBuffer = nullptr;
		};
		struct SceneResources
		{
			std::map<std::string, GeometryResources> m_geometryResources;
		};

	private:
		void LoadGeometryBuffers(int sceneIndex, const std::string& geometryName, SceneResources& sceneResources, jobs::Job* done);
		
		void LoadBuffers(int sceneIndex, jobs::Job* done);
	public:
		std::vector<collada::ColladaScene*> m_colladaScenes;
		std::vector<SceneResources> m_sceneResources;

		DXScene();
		virtual ~DXScene();

		void LoadColladaScene(const std::string& filePath, jobs::Job* done);

		void GetSceneBB(DirectX::XMVECTOR& minPoint, DirectX::XMVECTOR& maxPoint);
	};
}