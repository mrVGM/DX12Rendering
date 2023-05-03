#pragma once

#include <list>
#include <string>

class BaseObject;

class BaseObjectMeta
{
private:
	const BaseObjectMeta* m_parentClass = nullptr;

protected:
	BaseObjectMeta(const BaseObjectMeta* parentClass);
	BaseObjectMeta(const BaseObjectMeta& other) = delete;

public:
	bool IsChildOf(const BaseObjectMeta& other) const;
};
