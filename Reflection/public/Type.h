#pragma once

#include "Reflection.h"

#include "BaseObjectMeta.h"
#include "BaseObject.h"

#include "XMLReader.h"
#include "XMLWriter.h"

#include <list>
#include <sstream>

namespace xml_reader
{
	struct Node;
}

namespace xml_writer
{
	struct Node;
}

namespace reflection
{
	class XMLSerializable
	{
	public:
		virtual void ToXMLTree(xml_writer::Node& rootNode) const = 0;
		virtual void FromXMLTree(const xml_reader::Node& rootNode) = 0;
	};

	struct Property;

	enum ValueType
	{
		Bool,
		Int,
		Float,
		String,
		Struct,
		Class,
	};

	struct DataDef : public ObjectWithID, public XMLSerializable
	{
		friend struct DataDefPayload;
	private:
		std::string m_id;
		std::string m_name;
		ValueType m_type;

	protected:
		DataDef(const std::string& id);

		DataDef(const DataDef& other) = delete;
		DataDef& operator=(const DataDef& other) = delete;

	public:
		virtual ~DataDef();

		void SetName(const std::string& name);
		void SetValueType(const ValueType& valueType);

		const std::string& GetID() const override;
		const std::string& GetName() const;
		const ValueType& GetValueType() const;

		virtual void ToXMLTree(xml_writer::Node& rootNode) const override;
		virtual void FromXMLTree(const xml_reader::Node& rootNode) override;

		void StoreTypeInfo() const;
	};

	struct BoolType : public DataDef
	{
		BoolType();
	};

	struct IntType : public DataDef
	{
		IntType();
	};

	struct FloatType : public DataDef
	{
		FloatType();
	};

	struct StringType : public DataDef
	{
		StringType();
	};

	struct StructType : public DataDef
	{
		friend class StructTypePayload;
	private:
		const BaseObjectMeta& m_meta;
		std::list<Property> m_properties;

	public:
		StructType(const BaseObjectMeta& meta, const std::string& id);
		Property& AddProperty();

		const BaseObjectMeta& GetMeta() const;

		virtual void ToXMLTree(xml_writer::Node& rootNode) const override;
		virtual void FromXMLTree(const xml_reader::Node& rootNode) override;

		void PostDeserialize();
	};

	struct ClassType : public DataDef
	{
		const BaseObjectMeta& m_meta;

		ClassType(const BaseObjectMeta& meta, const std::string& id);
	};
}