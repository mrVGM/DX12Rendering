#include "AnimRepo.h"

#include "AnimRepoMeta.h"

#include "utils.h"

#include "CoreUtils.h"

namespace
{
	collada::SceneSettings* m_sceneSettings = nullptr;

	void CacheObjects()
	{
		if (!m_sceneSettings)
		{
			m_sceneSettings = animation::GetSceneSettings();
		}
	}
}

animation::AnimRepo::AnimRepo() :
	BaseObject(animation::AnimRepoMeta::GetInstance())
{
	CacheObjects();
}

animation::AnimRepo::~AnimRepo()
{
}

void animation::AnimRepo::LoadAnimations()
{
	collada::SceneSettings::Settings& settings = m_sceneSettings->GetSettings();

	struct Context
	{
		AnimRepo* m_self = nullptr;
		std::string m_name;
		collada::Animation m_animation;
	};

	class RegisterAnim : public jobs::Job
	{
	private:
		Context& m_ctx;
	public:
		RegisterAnim(Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			m_ctx.m_self->m_animations[m_ctx.m_name] = m_ctx.m_animation;
			delete &m_ctx;
		}
	};

	class LoadAnim : public jobs::Job
	{
	private:
		Context& m_ctx;
	public:
		LoadAnim(Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			const collada::SceneSettings::AnimationInfo& animInfo = m_sceneSettings->GetSettings().m_animations[m_ctx.m_name];

			data::MemoryFile mf;
			mf.RestoreFromFile(data::GetLibrary().GetRootDir() + animInfo.m_binFile);

			data::MemoryFileReader reader(mf);
			m_ctx.m_animation.Deserialize(reader);

			rendering::core::utils::RunSync(new RegisterAnim(m_ctx));
		}
	};

	for (auto it = settings.m_animations.begin(); it != settings.m_animations.end(); ++it)
	{
		Context* ctx = new Context();
		ctx->m_self = this;
		ctx->m_name = it->first;

		rendering::core::utils::RunAsync(new LoadAnim(*ctx));
	}
}

const collada::Animation* animation::AnimRepo::GetAnimation(const std::string& animationName)
{
	auto it = m_animations.find(animationName);

	if (it == m_animations.end())
	{
		return nullptr;
	}

	return &(it->second);
}
