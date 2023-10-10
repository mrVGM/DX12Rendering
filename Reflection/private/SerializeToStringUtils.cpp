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

reflection::ValueType reflection::ReadValueTypeFromXML(const xml_reader::Node* rootNode)
{
	using namespace xml_reader;
	const Node* typeNode = FindChildNode(rootNode, [](const Node* node) {
		if (node->m_tagName == "value_type")
		{
			return true;
		}

		return false;
	});

	std::string valueTypeStr = typeNode->m_data.front()->m_symbolData.m_string;
	return ValueTypeFromString(valueTypeStr);
}

std::string reflection::ReadIdFromXML(const xml_reader::Node* rootNode)
{
	using namespace xml_reader;
	const Node* idNode = FindChildNode(rootNode, [](const Node* node) {
		if (node->m_tagName == "id")
		{
			return true;
		}

		return false;
	});

	return idNode->m_data.front()->m_symbolData.m_string;
}
