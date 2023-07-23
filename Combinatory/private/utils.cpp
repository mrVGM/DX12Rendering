#include "utils.h"

#include "CombinatorySettingsMeta.h"

#include "BaseObjectContainer.h"


combinatory::CombinatorySettings* combinatory::GetSettings()
{
	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	BaseObject* obj = container.GetObjectOfClass(CombinatorySettingsMeta::GetInstance());

	if (!obj)
	{
		throw "Can't find Combinatory Settings!";
	}

	CombinatorySettings* settings = static_cast<CombinatorySettings*>(obj);
	return settings;
}

int combinatory::GCD(int a, int b)
{
	while (b != 0)
	{
		int t = b;
		b = a % b;
		a = t;
	}

	return a;
}
