#pragma once

#include "Reflection.h"
#include "Type.h"

namespace reflection
{
	struct DataDef;

	struct Property : public ObjectWithID
	{
	private:
		std::string m_id;
		std::string m_name;
		const DataDef* m_dataType = nullptr;

	public:
		Property();

		Property(const Property& other) = delete;
		Property& operator=(const Property& other) = delete;

		void SetName(const std::string name);
		void SetDataType(const DataDef& dataType);

		const std::string& GetID() const override;
		const std::string& GetName() const;
		const DataDef& GetDataType() const;

		void Init();
	};
}