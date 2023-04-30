#pragma once

#include <list>

class BaseObjectMeta;

class BaseObjectMetaContainer
{
	friend void BaseFrameworkShutdown();
private:
	std::list<const BaseObjectMeta*> m_metaRefs;
	static void Shutdown(); 

protected:

public:
	static BaseObjectMetaContainer& GetInstance();

	void RegisterMeta(const BaseObjectMeta* meta);
	const std::list<const BaseObjectMeta*>& GetAllMetas() const;

	BaseObjectMetaContainer();
};
