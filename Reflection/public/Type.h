#pragma once

#include "Reflection.h"

#include "BaseObjectMeta.h"

namespace reflection
{
	struct DataType : public ObjectWithID
	{
	public:
		enum Type
		{
			None,
			Int,
			Float,
			String,
			Struct,
			Class,
		};

	private:
		std::string m_id;
		Type m_type;
		std::string m_name;

	protected:
		DataType(const std::string& id, const Type& type);

		DataType(const DataType& other) = delete;
		DataType& operator=(const DataType& other) = delete;

		virtual ~DataType();
	public:
		const std::string& GetID() const override;

		void SetName(const std::string& name);
		const std::string& GetName() const;

		const Type& GetType() const;
	};

	struct IntType : public DataType
	{
		IntType();
	};

	struct FloatType : public DataType
	{
		FloatType();
	};

	struct StringType : public DataType
	{
		StringType();
	};

	struct StructType : public DataType
	{
		const BaseObjectMeta& m_meta;

		StructType(const BaseObjectMeta& meta);
	};

	struct ClassType : public DataType
	{
		const BaseObjectMeta& m_meta;

		ClassType(const BaseObjectMeta& meta);
	};
}