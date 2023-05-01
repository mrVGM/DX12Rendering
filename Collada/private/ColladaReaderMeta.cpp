#include "ColladaReaderMeta.h"

namespace
{
	collada::ColladaReaderMeta m_instance;
}

collada::ColladaReaderMeta::ColladaReaderMeta() :
	BaseObjectMeta(nullptr)
{
}

const collada::ColladaReaderMeta& collada::ColladaReaderMeta::GetInstance()
{
	return m_instance;
}