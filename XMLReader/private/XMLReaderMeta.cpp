#include "XMLReaderMeta.h"

namespace
{
	xml_reader::XMLReaderMeta m_instance;
}

xml_reader::XMLReaderMeta::XMLReaderMeta() :
	BaseObjectMeta(nullptr)
{
}

const xml_reader::XMLReaderMeta& xml_reader::XMLReaderMeta::GetInstance()
{
	return m_instance;
}