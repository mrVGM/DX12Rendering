#pragma once

#include "BaseObject.h"

#include "ColladaScene.h"
#include "Job.h"
#include "JobSystem.h"

#include <list>
#include <string>

class BaseObjectMeta;

namespace rendering
{
	class DXScene : public BaseObject
	{
	private:
		std::list<collada::ColladaScene*> m_colladaScenes;

	public:
		DXScene();
		virtual ~DXScene();

		void LoadColladaScene(const std::string& filePath, jobs::Job* done, jobs::JobSystem* jobSystem);
	};
}