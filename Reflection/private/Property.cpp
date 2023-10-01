#include "Property.h"

#include <Windows.h>
#include <sstream>

reflection::Property::Property()
{
}

void reflection::Property::SetName(const std::string name)
{
	m_name = name;
}

void reflection::Property::SetDataType(const DataDef& dataType)
{
	m_dataType = &dataType;
}

const std::string& reflection::Property::GetID() const
{
	return m_id;
}

const std::string& reflection::Property::GetName() const
{
	return m_name;
}

const reflection::DataDef& reflection::Property::GetDataType() const
{
	return *m_dataType;
}

void reflection::Property::Init()
{
	GUID guid;
	HRESULT hCreateGuid = CoCreateGuid(&guid);

	std::stringstream ss;
	
    ss << std::uppercase;
    ss.width(8);
    ss << std::hex << guid.Data1 << '-';

    ss.width(4);
    ss << std::hex << guid.Data2 << '-';

    ss.width(4);
    ss << std::hex << guid.Data3 << '-';

    ss.width(2);
    ss << std::hex
        << static_cast<short>(guid.Data4[0])
        << static_cast<short>(guid.Data4[1])
        << '-'
        << static_cast<short>(guid.Data4[2])
        << static_cast<short>(guid.Data4[3])
        << static_cast<short>(guid.Data4[4])
        << static_cast<short>(guid.Data4[5])
        << static_cast<short>(guid.Data4[6])
        << static_cast<short>(guid.Data4[7]);
    ss << std::nouppercase;

    m_id = ss.str();
}
