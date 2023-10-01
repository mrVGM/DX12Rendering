#pragma once

#include "Reflection.h"

#include "BaseObjectMeta.h"
#include "BaseObject.h"

#include <functional>

namespace reflection
{
	enum ValueType
	{
		Bool,
		Int,
		Float,
		String,
		Struct,
		Class,
	};

	struct DataDef : public ObjectWithID
	{
	private:
		std::string m_id;
		ValueType m_type;
		std::string m_name;

	protected:
		DataDef(const std::string& id);

		DataDef(const DataDef& other) = delete;
		DataDef& operator=(const DataDef& other) = delete;

		virtual ~DataDef();
	public:

		void SetName(const std::string& name);
		void SetValueType(const ValueType& valueType);

		const std::string& GetID() const override;
		const std::string& GetName() const;
		const ValueType& GetValueType() const;
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
		const BaseObjectMeta& m_meta;

		StructType(const BaseObjectMeta& meta);
	};

	struct ClassType : public DataDef
	{
		const BaseObjectMeta& m_meta;

		ClassType(const BaseObjectMeta& meta);
	};
}