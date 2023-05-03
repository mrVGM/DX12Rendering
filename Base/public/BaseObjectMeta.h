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

public:
	BaseObjectMeta(const BaseObjectMeta& other) = delete;
	BaseObjectMeta& operator=(const BaseObjectMeta& other) = delete;

	bool IsChildOf(const BaseObjectMeta& other) const;
};
