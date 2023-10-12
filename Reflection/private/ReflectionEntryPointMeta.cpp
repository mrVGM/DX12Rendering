#include "ReflectionEntryPointMeta.h"

#include "AppEntryPointMeta.h"
#include "ReflectionEntryPointMetaTag.h"

namespace
{
	reflection::ReflectionEntryPointMeta m_meta;
}

reflection::ReflectionEntryPointMeta::ReflectionEntryPointMeta() :
	BaseObjectMeta(&settings::AppEntryPointMeta::GetInstance())
{
	m_metaTags.insert(&settings::ReflectionEntryPointMetaTag::GetInstance());
}

const reflection::ReflectionEntryPointMeta& reflection::ReflectionEntryPointMeta::GetInstance()
{
	return m_meta;
}
