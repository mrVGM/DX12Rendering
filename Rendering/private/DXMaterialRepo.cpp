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

	void LoadDeferredMaterial()
	{
		using namespace rendering;

		struct Context
		{
			DXDeferredMaterial* m_material = nullptr;
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
				float color[] = { 1, 1, 0, 1 };
				buffer->CopyData(color, _countof(color) * sizeof(float));

				DXMaterialRepo* repo = utils::GetMaterialRepo();
				repo->Register("yellow", *m_ctx.m_material);
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
		utils::RunSync(new CreateDeferredMaterial(ctx));
	}

}

rendering::DXMaterialRepo::DXMaterialRepo() :
	BaseObject(rendering::DXMaterialRepoMeta::GetInstance())
{
	LoadErrorMaterial();
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
	rendering::shader_repo::LoadShaderPrograms();

	DXShader* ps = rendering::shader_repo::GetErrorPixelShader();
	DXShader* vs = rendering::shader_repo::GetMainVertexShader();

	new DXUnlitErrorMaterial(*vs, *ps);
	DXMaterial* errorMat = utils::GetUnlitErrorMaterial();

	DXMaterialRepo* repo = utils::GetMaterialRepo();
	Register("error", *errorMat);
}

void rendering::DXMaterialRepo::LoadMaterials()
{
	LoadCyanMaterial();
	LoadDeferredMaterial();
}