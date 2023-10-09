#include "Type.h"

#include "Property.h"
#include "TypeManager.h"

#include "SerializeToStringUtils.h"

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

void reflection::DataDef::ToXMLTree(xml_writer::Node& rootNode) const
{
	using namespace xml_writer;

	rootNode.m_tagName = "data-def";
	Node& idNode = rootNode.m_children.emplace_back();
	idNode.m_tagName = "id";
	idNode.m_content = EncodeAsString(m_id);

	Node& nameNode = rootNode.m_children.emplace_back();
	nameNode.m_tagName = "name";
	nameNode.m_content = EncodeAsString(m_name);

	Node& valueTypeNode = rootNode.m_children.emplace_back();
	valueTypeNode.m_tagName = "value-type";
	valueTypeNode.m_content = EncodeAsString(ValueTypeToString(m_type));
}

void reflection::DataDef::FromXMLTree(const xml_reader::Node& rootNode)
{
	using namespace xml_reader;

	const Node* idNode = FindChildNode(&rootNode, [](const Node* node) {
		if (node->m_tagName == "id")
		{
			return true;
		}

		return false;
	});

	const Node* nameNode = FindChildNode(&rootNode, [](const Node* node) {
		if (node->m_tagName == "name")
		{
			return true;
		}

		return false;
	});

	const Node* valueTypeNode = FindChildNode(&rootNode, [](const Node* node) {
		if (node->m_tagName == "value-type")
		{
			return true;
		}

		return false;
	});

	m_id = idNode->m_data.front()->m_symbolData.m_string;
	m_name = nameNode->m_data.front()->m_symbolData.m_string;
	m_type = ValueTypeFromString(valueTypeNode->m_data.front()->m_symbolData.m_string);
}