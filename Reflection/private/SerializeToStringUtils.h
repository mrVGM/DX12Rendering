#pragma once

#include <string>

#include "Type.h"

namespace reflection
{
	std::string VatueTypeToString(const ValueType& valueType);
	ValueType VatueTypeFromString(const std::string& str);
}