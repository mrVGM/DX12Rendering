#include "DXMaterialRepo.h"

#include "DXMaterialRepoMeta.h"

#include "DXMaterial.h"
#include "Materials/SharederRepo.h"
#include "Materials/DXUnlitErrorMaterial.h"
#include "Materials/DXUnlitMaterial.h"
#include "Materials/DXDeferredMaterial.h"

#include "RenderUtils.h"

namespace
{
	void LoadCyanMaterial()
	{
		using namespace rendering;

		struct Context
		{
			DXUnlitMaterial* m_material = nullptr;
		};

		class SettingsBufferReady : public jobs::Job
		{
		private:
			Context m_ctx;
		public:
			SettingsBufferReady(const Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				DXBuffer* buffer = m_ctx.m_material->GetSettingsBuffer();
				float color[] = { 0, 1, 1, 1 };
				buffer->CopyData(color, _countof(color) * sizeof(float));

				DXMaterialRepo* repo = utils::GetMaterialRepo();
				repo->Register("cyan", *m_ctx.m_material);
			}
		};

		class CreateUnlitMaterial : public jobs::Job
		{
		private:
			Context m_ctx;
		public:
			CreateUnlitMaterial(const Context& ctx) :
				m_ctx(ctx)
			{
			}
			void Do() override
			{
				m_ctx.m_material = new DXUnlitMaterial(*shader_repo::GetMainVertexShader(), *shader_repo::GetUnlitPixelShader());
				m_ctx.m_material->CreateSettingsBuffer(new SettingsBufferReady(m_ctx));
			}
		};

		Context ctx;
		utils::RunSync(new CreateUnlitMaterial(ctx));
	}

	void LoadMaterial(const collada::ColladaMaterial& material)
	{
		using namespace rendering;

		struct Context
		{
			DXDeferredMaterial* m_material = nullptr;
			const collada::ColladaMaterial* m_colladaMaterial = nullptr;
		};

		class SettingsBufferReady : public jobs::Job
		{
		private:
			Context m_ctx;
		public:
			SettingsBufferReady(const Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				DXBuffer* buffer = m_ctx.m_material->GetSettingsBuffer();
				float color[] =
				{
					m_ctx.m_colladaMaterial->m_diffuseColor[0],
					m_ctx.m_colladaMaterial->m_diffuseColor[1],
					m_ctx.m_colladaMaterial->m_diffuseColor[2],
					m_ctx.m_colladaMaterial->m_diffuseColor[3],

					0.3,
					0.3,
					0.3,
					64
				};
				buffer->CopyData(color, _countof(color) * sizeof(float));

				DXMaterialRepo* repo = utils::GetMaterialRepo();
				repo->Register(m_ctx.m_colladaMaterial->m_name, *m_ctx.m_material);
			}
		};

		class CreateDeferredMaterial : public jobs::Job
		{
		private:
			Context m_ctx;
		public:
			CreateDeferredMaterial(const Context& ctx) :
				m_ctx(ctx)
			{
			}
			void Do() override
			{
				m_ctx.m_material = new DXDeferredMaterial(*shader_repo::GetMainVertexShader(), *shader_repo::GetDeferredPixelShader());
				m_ctx.m_material->CreateSettingsBuffer(new SettingsBufferReady(m_ctx));
			}
		};

		Context ctx;
		ctx.m_colladaMaterial = &material;
		utils::RunSync(new CreateDeferredMaterial(ctx));
	}
}

rendering::DXMaterialRepo::DXMaterialRepo() :
	BaseObject(rendering::DXMaterialRepoMeta::GetInstance())
{
}

rendering::DXMaterialRepo::~DXMaterialRepo()
{
}

rendering::DXMaterial* rendering::DXMaterialRepo::GetMaterial(const std::string& name) const
{
	auto it = m_repo.find(name);
	if (it == m_repo.end())
	{
		return nullptr;
	}

	return it->second;
}


void rendering::DXMaterialRepo::Register(const std::string& name, rendering::DXMaterial& material)
{
	m_repo[name] = &material;
}

void rendering::DXMaterialRepo::LoadErrorMaterial()
{
	DXShader* ps = rendering::shader_repo::GetErrorPixelShader();
	DXShader* vs = rendering::shader_repo::GetMainVertexShader();

	DXMaterial* errorMat = new DXUnlitErrorMaterial(*vs, *ps);
	DXMaterialRepo* repo = utils::GetMaterialRepo();
	Register("error", *errorMat);
}

void rendering::DXMaterialRepo::EnableMaterialLoading()
{
	m_canLoadMaterials = true;

	LoadErrorMaterial();

	std::list<const collada::ColladaMaterial*> cache = m_colladaMaterialsToLoad;
	m_colladaMaterialsToLoad.clear();

	for (auto it = cache.begin(); it != cache.end(); ++it)
	{
		LoadMaterial(*(*it));
	}
}

void rendering::DXMaterialRepo::LoadColladaMaterial(const collada::ColladaMaterial& material)
{
	if (!m_canLoadMaterials)
	{
		m_colladaMaterialsToLoad.push_back(&material);
		return;
	}

	std::list<const collada::ColladaMaterial*> cache = m_colladaMaterialsToLoad;
	m_colladaMaterialsToLoad.clear();

	for (auto it = cache.begin(); it != cache.end(); ++it)
	{
		LoadMaterial(*(*it));
	}

	LoadMaterial(material);
}