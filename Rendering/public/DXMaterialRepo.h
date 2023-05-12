#pragma once

#include "BaseObject.h"

#include "ColladaEntities.h"

#include <map>
#include <list>
#include <string>

namespace rendering
{
	class DXMaterial;
	class DXMaterialRepo : public BaseObject
	{
	private:
		std::map<std::string, DXMaterial*> m_repo;
		void LoadErrorMaterial();
		bool m_canLoadMaterials = false;
		std::list<const collada::ColladaMaterial*> m_colladaMaterialsToLoad;

	public:
		DXMaterialRepo();
		virtual ~DXMaterialRepo();

		DXMaterial* GetMaterial(const std::string& name) const;
		void Register(const std::string& name, DXMaterial& material);

		void EnableMaterialLoading();
		void LoadColladaMaterial(const collada::ColladaMaterial& material);
	};
}