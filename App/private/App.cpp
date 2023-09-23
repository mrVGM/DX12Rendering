#include "App.h"

#include "Renderer.h"
#include "SceneConverter.h"

#include "RendererEntryPointMetaTag.h"
#include "SceneConverterEntryPointMetaTag.h"

#include "BaseObjectContainer.h"

#include "AppSettings.h"
#include "AppSettingsMeta.h"
#include "AppEntryPoint.h"
#include "AppEntryPointMeta.h"

#include "BaseFramework.h"

#include <map>

namespace
{
	std::map<std::string, const BaseObjectMetaTag*> m_appEntryPoints;

	void RegisterLibs()
	{
		rendering::RegisterLib();
		scene_converter::RegisterLib();

		m_appEntryPoints["Renderer"] = &settings::RendererEntryPointMetaTag::GetInstance();
		m_appEntryPoints["SceneConverter"] = &settings::SceneConverterEntryPointMetaTag::GetInstance();
	}
}


settings::AppEntryPoint* app::BootApp()
{
	RegisterLibs();

	BaseObjectContainer& container = BaseObjectContainer::GetInstance();
	const settings::AppSettings* settings = nullptr;
	{
		BaseObject* tmp = container.GetObjectOfClass(settings::AppSettingsMeta::GetInstance());
		settings = static_cast<settings::AppSettings*>(tmp);
	}

	std::list<BaseObject*> tmp;
	container.GetAllObjectsOfClass(settings::AppEntryPointMeta::GetInstance(), tmp);

	const std::string& entryPoint = settings->GetSettings().m_appEntryPoint;
	const BaseObjectMetaTag* entryPointTag = m_appEntryPoints.find(entryPoint)->second;

	for (auto it = tmp.begin(); it != tmp.end(); ++it)
	{
		settings::AppEntryPoint* entryPoint = static_cast<settings::AppEntryPoint*>(*it);
		if (entryPoint->GetMeta().HasTag(*entryPointTag))
		{
			entryPoint->Boot();
			return entryPoint;
		}
	}

	return nullptr;
}

void app::ShutdownApp()
{
	BaseObjectContainer::GetInstance().StopExclusiveThreadAccess();
	BaseFrameworkShutdown();
}