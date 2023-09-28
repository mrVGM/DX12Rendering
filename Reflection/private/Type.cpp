#include "Type.h"

#include "TypeManager.h"

reflection::DataType::DataType(const std::string& id, const Type& type) :
	m_id(id),
	m_type(type)
{
	TypeManager& typeManager = TypeManager::GetInstance();
	typeManager.RegisterType(*this);
}

reflection::DataType::~DataType()
{
}

const std::string& reflection::DataType::GetID() const
{
	return m_id;
}

void reflection::DataType::SetName(const std::string& name)
{
	m_name = name;
}

const std::string& reflection::DataType::GetName() const
{
	return m_name;
}

const reflection::DataType::Type& reflection::DataType::GetType() const
{
	return m_type;
}

reflection::IntType::IntType() :
	DataType("205822CA-AFE5-4B94-B147-01E054F1C024", DataType::Int)
{
	SetName("int");
}

reflection::FloatType::FloatType() :
	DataType("A5935EC8-E57C-4F54-8C1A-AF4058247271", DataType::Float)
{
	SetName("float");
}

reflection::StringType::StringType() :
	DataType("B97E49D8-2807-4842-9EEA-980126B26792", DataType::String)
{
	SetName("string");
}

reflection::StructType::StructType(const BaseObjectMeta& meta) :
	DataType("8AA14012-9528-4B7B-94DE-D0FA54C44D7B", DataType::Struct),
	m_meta(meta)
{
}

reflection::ClassType::ClassType(const BaseObjectMeta& meta) :
	DataType("3FE53B6A-31F1-4042-A427-7962E24E19BC", DataType::Class),
	m_meta(meta)
{
}

reflection::Property::Property(
	const std::string& id,
	const DataType& dataType,
	const Accessibility& accessiblity,
	const std::function<void* (BaseObject*)> accessor) :
	m_id(id),
	m_type(dataType),
	m_accessibility(accessiblity),
	m_accessor(accessor)
{
}

const std::string& reflection::Property::GetID() const
{
	return m_id;
}

void* reflection::Property::GetMemoryAddess(BaseObject* object)
{
	return m_accessor(object);
}

void reflection::Property::SetName(const std::string& name)
{
	m_name = name;
}

const std::string& reflection::Property::GetName() const
{
	return m_name;
}