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

	public:
		void RegisterType(const DataDef& type);

		const DataDef* GetType(const std::string& id) const;
		void GetTypes(const ValueType& type, std::list<const DataDef*>& outTypes) const;

		static TypeManager& GetInstance();
	};
}