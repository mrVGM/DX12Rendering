#pragma once

#include "BaseObject.h"

#include "ColladaScene.h"
#include "Job.h"
#include "JobSystem.h"

#include <vector>
#include <string>
#include <map>

class BaseObjectMeta;

namespace rendering
{
	class DXBuffer;
	class DXScene : public BaseObject
	{
	public:
		struct SceneResources
		{
			std::map<std::string, DXBuffer*> m_vertexBuffers;
			std::map<std::string, DXBuffer*> m_indexBuffers;
		};

	private:
		std::vector<collada::ColladaScene*> m_colladaScenes;
		std::vector<SceneResources> m_sceneResources;

	public:
		DXScene();
		virtual ~DXScene();

		void LoadColladaScene(const std::string& filePath, jobs::Job* done);
		void LoadVertexBuffers(int sceneIndex, jobs::Job* done);
	};
}