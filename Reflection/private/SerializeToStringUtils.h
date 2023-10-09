#pragma once

#include <string>

#include "Type.h"
#include "Property.h"

namespace reflection
{
	std::string ValueTypeToString(const ValueType& valueType);
	ValueType ValueTypeFromString(const std::string& str);

	std::string AccessTypeToString(const AccessType& accessType);
	AccessType AccessTypeFromString(const std::string& str);

	std::string StructureTypeToString(const StructureType& structureType);
	StructureType StructureTypeFromString(const std::string& str);
}