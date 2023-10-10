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

	struct DataDefPayload : public XMLSerializable
	{
		std::string m_id;
		std::string m_name;
		ValueType m_type;

		virtual void ToXMLTree(xml_writer::Node& rootNode) const override;
		virtual void FromXMLTree(const xml_reader::Node& rootNode) override;

		virtual void InitTypeDef(DataDef& dataDef) const;
	};

	struct StructTypePayload : public DataDefPayload
	{
		std::list<Property> m_properties;

		void ToXMLTree(xml_writer::Node& rootNode) const override;
		void FromXMLTree(const xml_reader::Node& rootNode) override;

		virtual void InitTypeDef(DataDef& dataDef) const;
	};
}