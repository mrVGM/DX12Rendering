#include "MaterialUtils.h"

#include "DXDeferredMaterial.h"
#include "Job.h"

#include "DXBuffer.h"
#include "DXMaterialRepo.h"

#include "RenderUtils.h"

#include "ShaderRepo.h"


#include <list>


namespace
{
	bool m_materialLoadingEnabled = false;
	std::list<const collada::ColladaMaterial*> m_materialsToLoad;

	void LoadErrorMaterial()
	{
		using namespace rendering;

		DXShader* ps = rendering::shader_repo::GetErrorPixelShader();
		DXShader* vs = rendering::shader_repo::GetMainVertexShader();

		DXMaterial* errorMat = new DXUnlitErrorMaterial(*vs, *ps);
		DXMaterialRepo* repo = utils::GetMaterialRepo();
		repo->Register("error", *errorMat);
	}
}

void rendering::material_utils::LoadMaterial(const collada::ColladaMaterial& material)
{
	using namespace rendering;

	if (!m_materialLoadingEnabled)
	{
		m_materialsToLoad.push_back(&material);
		return;
	}

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

void rendering::material_utils::EnableMaterialLoading()
{
	std::list<const collada::ColladaMaterial*> cache = m_materialsToLoad;
	m_materialsToLoad.clear();
	m_materialLoadingEnabled = true;
	
	LoadErrorMaterial();
	
	for (auto it = cache.begin(); it != cache.end(); ++it)
	{
		LoadMaterial(*(*it));
	}
}
