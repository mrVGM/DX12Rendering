#include "ShadowMapping.h"

#include "CascadedSM.h"
#include "PSM.h"

#include "AppSettings.h"

namespace
{
	rendering::shadow_mapping::ShadowMap* m_shadowMap = nullptr;
}

rendering::shadow_mapping::ShadowMap* rendering::shadow_mapping::GetShadowMap()
{
	if (m_shadowMap)
	{
		return m_shadowMap;
	}

	settings::AppSettings* appSettings = settings::AppSettings::GetAppSettings();

	const settings::AppSettings::Settings& settings = appSettings->GetSettings();
	if (settings.m_shadowMapType == "Cascaded")
	{
		m_shadowMap = new CascadedSM();
		return m_shadowMap;
	}
	if (settings.m_shadowMapType == "PSM")
	{
		m_shadowMap = new psm::PSM();
		return m_shadowMap;
	}

	return nullptr;
}