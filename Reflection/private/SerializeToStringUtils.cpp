#include "SerializeToStringUtils.h"

std::string reflection::VatueTypeToString(const ValueType& valueType)
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

reflection::ValueType reflection::VatueTypeFromString(const std::string& str)
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
