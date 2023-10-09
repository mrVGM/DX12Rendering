#pragma once

#include "Type.h"

#include <map>
#include <list>

namespace reflection
{
	class TypeManager
	{
	private:
		std::map<std::string, const DataDef*> m_types;
		std::list<const BaseObjectMeta*> m_generatedTypeMetas;
		std::list<const DataDef*> m_generatedTypes;

	public:
		static TypeManager& GetInstance();

		~TypeManager();

		void RegisterType(const DataDef& type);

		const DataDef* GetType(const std::string& id) const;
		void GetTypes(const ValueType& type, std::list<const DataDef*>& outTypes) const;

		void RegisterGeneratedType(const BaseObjectMeta& meta, const DataDef& type);
		void LoadGeneratedTypes();
	};
}