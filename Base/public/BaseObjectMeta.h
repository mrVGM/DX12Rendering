#pragma once

#include <set>

class BaseObjectMetaTag;

class BaseObjectMeta
{
private:
	const BaseObjectMeta* m_parentClass = nullptr;
	std::set<const BaseObjectMetaTag*> m_metaTags;
protected:
	BaseObjectMeta(const BaseObjectMeta* parentClass, int numMetaTags = 0, const BaseObjectMetaTag*const* metaTags = nullptr);

public:
	BaseObjectMeta(const BaseObjectMeta& other) = delete;
	BaseObjectMeta& operator=(const BaseObjectMeta& other) = delete;

	bool IsChildOf(const BaseObjectMeta& other) const;
	bool HasTag(const BaseObjectMetaTag& metaTag) const;
};
