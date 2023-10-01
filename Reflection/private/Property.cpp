#include "Property.h"

#include "utils.h"

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

void reflection::Property::SetMapValueDataType(const DataDef& dataType)
{
    m_mapValueDataType = &dataType;
}

void reflection::Property::SetAccessType(const AccessType& accessType)
{
    m_accessType = accessType;
}

void reflection::Property::SetStructureType(const StructureType& structureType)
{
    m_structureType = structureType;
}

void reflection::Property::SetAddressAccessor(const std::function<void* (BaseObject&)>& accessor)
{
    m_addressAccessor = accessor;
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

const reflection::DataDef& reflection::Property::GetMapValueDataType() const
{
    return *m_mapValueDataType;
}

const reflection::AccessType& reflection::Property::GetAccessType() const
{
    return m_accessType;
}

const reflection::StructureType& reflection::Property::GetStructureType() const
{
    return m_structureType;
}

void reflection::Property::Init()
{
    m_id = GetNewId();
}

void* reflection::Property::GetAddress(BaseObject& object) const
{
    void* address = m_addressAccessor(object);
    return address;
}
