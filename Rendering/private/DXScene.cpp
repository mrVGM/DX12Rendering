#include "DXScene.h"

#include "DXSceneMeta.h"

#include "RenderUtils.h"

rendering::DXScene::DXScene() :
	BaseObject(DXSceneMeta::GetInstance())
{
}

rendering::DXScene::~DXScene()
{
}

void rendering::DXScene::LoadColladaScene(const std::string& filePath, jobs::Job* done, jobs::JobSystem* jobSystem)
{
	struct JobContext
	{
		DXScene* m_dxScene = nullptr;
		std::string m_filePath;
		collada::ColladaScene* m_scene = nullptr;
		jobs::Job* m_done = nullptr;
		jobs::JobSystem* m_jobSystem = nullptr;
	};

	class PostLoadColladaSceneJob : public jobs::Job
	{
	private:
		JobContext m_context;
	public:
		PostLoadColladaSceneJob(const JobContext& context) :
			m_context(context)
		{
		}

		void Do() override
		{
			m_context.m_dxScene->m_colladaScenes.push_back(m_context.m_scene);

			m_context.m_jobSystem->ScheduleJob(m_context.m_done);
		}
	};

	class LoadColladaSceneJob : public jobs::Job
	{
	private:
		JobContext m_context;
	public:
		LoadColladaSceneJob(const JobContext& context) :
			m_context(context)
		{
		}

		void Do() override
		{
			m_context.m_scene->Load(m_context.m_filePath);

			jobs::JobSystem* mainSystem = utils::GetMainJobSystem();
			mainSystem->ScheduleJob(new PostLoadColladaSceneJob(m_context));
		}
	};

	class CreateColladaSceneJob : public jobs::Job
	{
	private:
		JobContext m_context;
	public:
		CreateColladaSceneJob(const JobContext& context) :
			m_context(context)
		{
		}

		void Do() override
		{
			m_context.m_scene = new collada::ColladaScene();

			jobs::JobSystem* loadSystem = utils::GetLoadJobSystem();
			loadSystem->ScheduleJob(new LoadColladaSceneJob(m_context));
		}
	};

	JobContext ctx{ this, filePath, nullptr, done, jobSystem };

	jobs::JobSystem* mainSystem = utils::GetMainJobSystem();
	mainSystem->ScheduleJob(new CreateColladaSceneJob(ctx));
}