#include "Property.h"

#include "utils.h"

#include "XMLReader.h"
#include "XMLWriter.h"

#include "SerializeToStringUtils.h"

#include "TypeManager.h"

#include <Windows.h>
#include <sstream>

reflection::Property::Property()
{
}

void reflection::Property::SetName(const std::string name)
{
	m_name = name;
}

void reflection::Property::SetDataType(const DataDef& dataType)
{
	m_dataType = &dataType;
    m_dataTypeId = m_dataType->GetID();
}

void reflection::Property::SetMapValueDataType(const DataDef& dataType)
{
    m_mapValueDataType = &dataType;
}

void reflection::Property::SetAccessType(const AccessType& accessType)
{
    m_accessType = accessType;
}

void reflection::Property::SetStructureType(const StructureType& structureType)
{
    m_structureType = structureType;
}

void reflection::Property::SetAddressAccessor(const std::function<void* (BaseObject&)>& accessor)
{
    m_addressAccessor = accessor;
}

const std::string& reflection::Property::GetID() const
{
	return m_id;
}

const std::string& reflection::Property::GetName() const
{
	return m_name;
}

const reflection::DataDef& reflection::Property::GetDataType() const
{
	return *m_dataType;
}

const reflection::DataDef& reflection::Property::GetMapValueDataType() const
{
    return *m_mapValueDataType;
}

const reflection::AccessType& reflection::Property::GetAccessType() const
{
    return m_accessType;
}

const reflection::StructureType& reflection::Property::GetStructureType() const
{
    return m_structureType;
}

void reflection::Property::Init()
{
    m_id = GetNewId();
}

void* reflection::Property::GetAddress(BaseObject& object) const
{
    void* address = m_addressAccessor(object);
    return address;
}

void reflection::Property::ToXMLTree(xml_writer::Node& rootNode) const
{
    using namespace xml_writer;

    rootNode.m_tagName = "property";

    {
        Node& idNode = rootNode.m_children.emplace_back();
        idNode.m_tagName = "id";
        idNode.m_content = EncodeAsString(m_id);
    }

    {
        Node& nameNode = rootNode.m_children.emplace_back();
        nameNode.m_tagName = "name";
        nameNode.m_content = EncodeAsString(m_name);
    }
    
    {
        Node& dataTypeId = rootNode.m_children.emplace_back();
        dataTypeId.m_tagName = "data_type_id";
        dataTypeId.m_content = EncodeAsString(m_dataTypeId);
    }

    if (!m_mapValueDataTypeId.empty())
    {
        Node& mapValueDataTypeIdNode = rootNode.m_children.emplace_back();
        mapValueDataTypeIdNode.m_tagName = "map_value_data_type_id";
        mapValueDataTypeIdNode.m_content = EncodeAsString(m_mapValueDataTypeId);
    }

    {
        Node& structureTypeNode = rootNode.m_children.emplace_back();
        structureTypeNode.m_tagName = "structure_type";
        structureTypeNode.m_content = EncodeAsString(StructureTypeToString(m_structureType));
    }

    {
        Node& accessTypeNode = rootNode.m_children.emplace_back();
        accessTypeNode.m_tagName = "access_type";
        accessTypeNode.m_content = EncodeAsString(AccessTypeToString(m_accessType));
    }

    {
        std::stringstream ss;

        Node& objectOffsetNode = rootNode.m_children.emplace_back();
        objectOffsetNode.m_tagName = "object_offset";
        ss << m_objectOffset;
        objectOffsetNode.m_content = ss.str();
    }
}

void reflection::Property::FromXMLTree(const xml_reader::Node& rootNode)
{
    using namespace xml_reader;

    {
        const Node* n = FindChildNode(&rootNode, [](const Node* node) {
            if (node->m_tagName == "id")
            {
                return true;
            }

            return false;
        });

        m_id = n->m_data.front()->m_symbolData.m_string;
    }

    {
        const Node* n = FindChildNode(&rootNode, [](const Node* node) {
            if (node->m_tagName == "name")
            {
                return true;
            }

            return false;
        });

        m_name = n->m_data.front()->m_symbolData.m_string;
    }

    {
        const Node* n = FindChildNode(&rootNode, [](const Node* node) {
            if (node->m_tagName == "data_type_id")
            {
                return true;
            }

            return false;
        });

        m_dataTypeId = n->m_data.front()->m_symbolData.m_string;
    }

    {
        const Node* n = FindChildNode(&rootNode, [](const Node* node) {
            if (node->m_tagName == "map_value_data_type_id")
            {
                return true;
            }

            return false;
        });

        if (n) 
        {
            m_mapValueDataTypeId = n->m_data.front()->m_symbolData.m_string;
        }
    }

    {
        const Node* n = FindChildNode(&rootNode, [](const Node* node) {
            if (node->m_tagName == "structure_type")
            {
                return true;
            }

            return false;
        });

        m_structureType = StructureTypeFromString(n->m_data.front()->m_symbolData.m_string);
    }

    {
        const Node* n = FindChildNode(&rootNode, [](const Node* node) {
            if (node->m_tagName == "access_type")
            {
                return true;
            }

            return false;
        });

        m_accessType = AccessTypeFromString(n->m_data.front()->m_symbolData.m_string);
    }

    {
        const Node* n = FindChildNode(&rootNode, [](const Node* node) {
            if (node->m_tagName == "object_offset")
            {
                return true;
            }

            return false;
        });

        m_objectOffset = static_cast<int>(n->m_data.front()->m_symbolData.m_number);
    }
}


void reflection::Property::PostDeserialize()
{
    TypeManager& typeManager = TypeManager::GetInstance();
    m_dataType = typeManager.GetType(m_dataTypeId);

    if (m_structureType == StructureType::Map)
    {
        m_mapValueDataType = typeManager.GetType(m_mapValueDataTypeId);
    }

    m_addressAccessor = [&](BaseObject& obj) {
        char* address = reinterpret_cast<char*>(&obj);
        address += m_objectOffset;
        return address;
    };
}