#pragma once

#include "Reflection.h"
#include "Type.h"

#include <functional>

namespace reflection
{
	struct DataDef;

	enum AccessType
	{
		Public,
		Protected,
		Private
	};

	enum StructureType
	{
		Single,
		Array,
		Set,
		Map,
		ClassDef
	};

	struct Property : public ObjectWithID, public XMLSerializable
	{
	private:
		std::string m_id;
		std::string m_name;

		std::string m_dataTypeId;
		const DataDef* m_dataType = nullptr;
		std::string m_mapValueDataTypeId;
		const DataDef* m_mapValueDataType = nullptr;

		StructureType m_structureType = StructureType::Single;
		AccessType m_accessType = AccessType::Public;

		int m_objectOffset = -1;
		std::function<void* (BaseObject&)> m_addressAccessor;

	public:
		Property();

		void SetName(const std::string name);
		void SetDataType(const DataDef& dataType);
		void SetMapValueDataType(const DataDef& dataType);
		void SetAccessType(const AccessType& accessType);
		void SetStructureType(const StructureType& structureType);
		void SetAddressAccessor(const std::function<void* (BaseObject&)>& accessor);

		const std::string& GetID() const override;
		const std::string& GetName() const;
		const DataDef& GetDataType() const;
		const DataDef& GetMapValueDataType() const;
		const AccessType& GetAccessType() const;
		const StructureType& GetStructureType() const;

		void Init();

		void* GetAddress(BaseObject& object) const;


		void ToXMLTree(xml_writer::Node& rootNode) const override;
		void FromXMLTree(const xml_reader::Node& rootNode) override;

		void PostDeserialize();
	};
}