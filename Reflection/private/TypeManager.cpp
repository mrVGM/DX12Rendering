#include "TypeManager.h"

#include "utils.h"

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

void reflection::TypeManager::RegisterGeneratedType(const BaseObjectMeta& meta, const DataDef& type)
{
	m_generatedTypeMetas.push_back(&meta);
	m_generatedTypes.push_back(&type);
}