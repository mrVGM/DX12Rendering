#include "SerializeToStringUtils.h"

std::string reflection::ValueTypeToString(const ValueType& valueType)
{
	switch (valueType)
	{
	case ValueType::Bool:
		return "bool";

	case ValueType::Int:
		return "int";

	case ValueType::Float:
		return "float";

	case ValueType::String:
		return "string";

	case ValueType::Struct:
		return "struct";

	case ValueType::Class:
		return "class";
	}

	return "Unknown";
}

reflection::ValueType reflection::ValueTypeFromString(const std::string& str)
{
	if (str == "bool") {
		return ValueType::Bool;
	}
	if (str == "int") {
		return ValueType::Int;
	}

	if (str == "float") {
		return ValueType::Float;
	}
	
	if (str == "string") {
		return ValueType::String;
	}
	
	if(str == "struct") {
		return ValueType::Struct;
	}

	if(str == "class") {
		return ValueType::Class;
	}

	return ValueType::Bool;
}

std::string reflection::AccessTypeToString(const AccessType& accessType)
{
	switch (accessType)
	{
	case AccessType::Private:
		return "private";

	case AccessType::Protected:
		return "protected";

	case AccessType::Public:
		return "public";
	}

	return "Unknown";
}

reflection::AccessType reflection::AccessTypeFromString(const std::string& str)
{
	if (str == "private") {
		return AccessType::Private;
	}
	if (str == "protected") {
		return AccessType::Protected;
	}

	if (str == "public") {
		return AccessType::Public;
	}

	return AccessType::Private;
}

std::string reflection::StructureTypeToString(const StructureType& structureType)
{
	switch (structureType)
	{
	case StructureType::Single:
		return "single";

	case StructureType::Array:
		return "array";
	
	case StructureType::Set:
		return "set";

	case StructureType::Map:
		return "map";

	case StructureType::ClassDef:
		return "class_def";
	}

	return "Unknown";
}

reflection::StructureType reflection::StructureTypeFromString(const std::string& str)
{
	if (str == "single") {
		return StructureType::Single;
	}
	if (str == "array") {
		return StructureType::Array;
	}
	if (str == "set") {
		return StructureType::Set;
	}
	if (str == "map") {
		return StructureType::Map;
	}
	if (str == "class_def") {
		return StructureType::ClassDef;
	}

	return StructureType::Single;
}

void reflection::DataDefPayload::ToXMLTree(xml_writer::Node& rootNode) const
{
	using namespace xml_writer;

	rootNode.m_tagName = "data_def";
	Node& idNode = rootNode.m_children.emplace_back();
	idNode.m_tagName = "id";
	idNode.m_content = EncodeAsString(m_id);

	Node& nameNode = rootNode.m_children.emplace_back();
	nameNode.m_tagName = "name";
	nameNode.m_content = EncodeAsString(m_name);

	Node& valueTypeNode = rootNode.m_children.emplace_back();
	valueTypeNode.m_tagName = "value_type";
	valueTypeNode.m_content = EncodeAsString(ValueTypeToString(m_type));
}

void reflection::DataDefPayload::FromXMLTree(const xml_reader::Node& rootNode)
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
		if (node->m_tagName == "value_type")
		{
			return true;
		}

		return false;
	});

	m_id = idNode->m_data.front()->m_symbolData.m_string;
	m_name = nameNode->m_data.front()->m_symbolData.m_string;
	m_type = ValueTypeFromString(valueTypeNode->m_data.front()->m_symbolData.m_string);
}

void reflection::DataDefPayload::InitTypeDef(DataDef& dataDef) const
{
	dataDef.m_id = m_id;
	dataDef.m_name = m_name;
	dataDef.m_type = m_type;
}

void reflection::StructTypePayload::ToXMLTree(xml_writer::Node& rootNode) const
{
	using namespace xml_writer;

	DataDefPayload::ToXMLTree(rootNode);

	Node& props = rootNode.m_children.emplace_back();
	props.m_tagName = "properties";

	for (auto it = m_properties.begin(); it != m_properties.end(); ++it)
	{
		Node& node = props.m_children.emplace_back();
		const Property& cur = *it;
		cur.ToXMLTree(node);
	}
}

void reflection::StructTypePayload::FromXMLTree(const xml_reader::Node& rootNode)
{
	using namespace xml_reader;

	DataDefPayload::FromXMLTree(rootNode);

	const Node* propertiesNode = FindChildNode(&rootNode, [](const Node* node) {
		if (node->m_tagName == "properties")
		{
			return true;
		}

		return false;
	});

	std::list<const Node*> properties;
	FindChildNodes(propertiesNode, [](const Node* node) {
		if (node->m_tagName == "property")
		{
			return true;
		}

		return false;
	}, properties);

	for (auto it = properties.begin(); it != properties.end(); ++it)
	{
		const Node* cur = *it;

		Property& prop = m_properties.emplace_back();
		prop.FromXMLTree(*cur);
	}
}

void reflection::StructTypePayload::InitTypeDef(DataDef& dataDef) const
{
	DataDefPayload::InitTypeDef(dataDef);

	StructType& structType = static_cast<StructType&>(dataDef);
	structType.m_properties = m_properties;
}

