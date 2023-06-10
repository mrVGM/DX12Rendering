#include "DXMaterialRepo.h"

#include "DXMaterialRepoMeta.h"

#include "DXMaterial.h"

#include "Notifications.h"
#include "MaterialResisteredNotificationMeta.h"

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
	notifications::Notify(MaterialResisteredNotificationMeta::GetInstance());
}
