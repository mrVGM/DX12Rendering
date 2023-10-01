#include "Type.h"

#include "Property.h"
#include "TypeManager.h"

reflection::DataDef::DataDef(const std::string& id) :
	m_id(id)
{
	TypeManager& typeManager = TypeManager::GetInstance();
	typeManager.RegisterType(*this);
}

reflection::DataDef::~DataDef()
{
}

const std::string& reflection::DataDef::GetID() const
{
	return m_id;
}

void reflection::DataDef::SetName(const std::string& name)
{
	m_name = name;
}

void reflection::DataDef::SetValueType(const ValueType& valueType)
{
	m_type = valueType;
}

const std::string& reflection::DataDef::GetName() const
{
	return m_name;
}

const reflection::ValueType& reflection::DataDef::GetValueType() const
{
	return m_type;
}

reflection::IntType::IntType() :
	DataDef("205822CA-AFE5-4B94-B147-01E054F1C024")
{
	SetName("int");
	SetValueType(ValueType::Int);
}

reflection::FloatType::FloatType() :
	DataDef("A5935EC8-E57C-4F54-8C1A-AF4058247271")
{
	SetName("float");
	SetValueType(ValueType::Float);
}

reflection::StringType::StringType() :
	DataDef("B97E49D8-2807-4842-9EEA-980126B26792")
{
	SetName("string");
	SetValueType(ValueType::String);
}

reflection::StructType::StructType(const BaseObjectMeta& meta, const std::string& id) :
	DataDef(id),
	m_meta(meta)
{
	SetValueType(ValueType::Struct);
}

reflection::ClassType::ClassType(const BaseObjectMeta& meta, const std::string& id) :
	DataDef(id),
	m_meta(meta)
{
	SetValueType(ValueType::Class);
}

reflection::Property& reflection::StructType::AddProperty()
{
	Property& prop = m_properties.emplace_back();
	prop.Init();
	return prop;
}