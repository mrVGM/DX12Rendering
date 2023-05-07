#pragma once

#include "BaseObject.h"

#include <map>
#include <string>

namespace rendering
{
	class DXMaterial;
	class DXMaterialRepo : public BaseObject
	{
	private:
		std::map<std::string, DXMaterial*> m_repo;
	public:
		DXMaterialRepo();
		virtual ~DXMaterialRepo();

		DXMaterial* GetMaterial(const std::string& name) const;
		void Register(const std::string& name, DXMaterial& material);
	};
}