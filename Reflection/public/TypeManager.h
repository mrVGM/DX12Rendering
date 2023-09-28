#pragma once

#include "Type.h"

#include <map>
#include <list>

namespace reflection
{
	class TypeManager
	{
	private:
		std::map<std::string, const DataType*> m_types;

	public:
		void RegisterType(const DataType& type);

		const DataType* GetType(const std::string& id) const;
		void GetTypes(const DataType::Type& type, std::list<const DataType*>& outTypes) const;

		static TypeManager& GetInstance();
	};
}