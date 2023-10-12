#include "TypeManager.h"

#include "SettingsReader.h"
#include "SettingsReaderMeta.h"

#include "DataDefReader.h"

#include "SerializeToStringUtils.h"

#include "ScriptingStructMeta.h"
#include "GeneratedObjectMeta.h"

#include "utils.h"

#include <iostream>

namespace
{
	class TypeManagerHolder
	{
	private:
		reflection::TypeManager* m_typeManager = nullptr;

	public:
		reflection::TypeManager& GetTypeManager()
		{
			if (!m_typeManager)
			{
				m_typeManager = new reflection::TypeManager();
			}

			return *m_typeManager;
		}

		~TypeManagerHolder()
		{
			if (m_typeManager)
			{
				delete m_typeManager;
				m_typeManager = nullptr;
			}
		}
	};


	TypeManagerHolder m_typeManagerHolder;
}

reflection::TypeManager::~TypeManager()
{
	for (auto it = m_generatedTypeMetas.begin(); it != m_generatedTypeMetas.end(); ++it)
	{
		const BaseObjectMeta* cur = *it;
		delete cur;
	}

	for (auto it = m_generatedTypes.begin(); it != m_generatedTypes.end(); ++it)
	{
		const DataDef* cur = *it;
		delete cur;
	}
}

void reflection::TypeManager::RegisterType(const DataDef& type)
{
	m_types[type.GetID()] = &type;
}

const reflection::DataDef* reflection::TypeManager::GetType(const std::string& id) const
{
	auto it = m_types.find(id);

	if (it == m_types.end())
	{
		return nullptr;
	}

	return it->second;
}

void reflection::TypeManager::GetTypes(std::list<const DataDef*>& outTypes) const
{
	for (auto it = m_types.begin(); it != m_types.end(); ++it)
	{
		outTypes.push_back(it->second);
	}
}

void reflection::TypeManager::GetTypes(const ValueType& type, std::list<const DataDef*>& outTypes) const
{
	for (auto it = m_types.begin(); it != m_types.end(); ++it)
	{
		if (it->second->GetValueType() == type)
		{
			outTypes.push_back(it->second);
		}
	}
}

reflection::TypeManager& reflection::TypeManager::GetInstance()
{
	return m_typeManagerHolder.GetTypeManager();
}

reflection::TypeManager::TypeManager()
{
}

void reflection::TypeManager::RegisterGeneratedType(const BaseObjectMeta& meta, const DataDef& type)
{
	m_generatedTypeMetas.push_back(&meta);
	m_generatedTypes.push_back(&type);
}

void reflection::TypeManager::StoreTypeInformation()
{
	for (auto it = m_types.begin(); it != m_types.end(); ++it)
	{
		it->second->StoreTypeInfo();
		std::cout << it->second->GetName() << " information written" << std::endl;
	}
}

void reflection::TypeManager::LoadGeneratedTypes()
{
	ReflectionSettings* reflectionSettings = GetReflectionSettings();
	ReflectionSettings::Settings& settings = reflectionSettings->GetSettings();

	DataDefReader* dataDefReader = new DataDefReader();

	std::list<StructType*> generated;
	StructType* structCache = nullptr;
	for (auto it = settings.m_files.begin(); it != settings.m_files.end(); ++it)
	{
		StructTypePayload tmp;
		dataDefReader->ParseXMLStruct(it->second, tmp);

		GeneratedObjectMeta* structTypeMeta = new GeneratedObjectMeta(scripting::ScriptingStructMeta::GetInstance());
		StructType* structType = new StructType(*structTypeMeta, tmp.m_id);
		structCache = structType;

		tmp.InitTypeDef(*structType);
		RegisterGeneratedType(*structTypeMeta, *structType);

		generated.push_back(structType);
	}

	for (auto it = generated.begin(); it != generated.end(); ++it)
	{
		StructType* cur = *it;
		cur->PostDeserialize();
	}
}