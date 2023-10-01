#include "CommandReader.h"

#include "TypeManager.h"

#include <sstream>

std::string frontend::CommandReader::ProcessMessage(const std::string& message)
{
	if (message == ":ping")
	{
		return ":ping";
	}

	if (message == ":quit")
	{
		return ":quit";
	}

	if (message == ":types")
	{
		reflection::TypeManager& typeManager = reflection::TypeManager::GetInstance();

		std::list<const reflection::DataDef*> types;
		typeManager.GetTypes(reflection::ValueType::Int, types);

		std::stringstream ss;
		for (auto it = types.begin(); it != types.end(); ++it)
		{
			ss << (*it)->GetName() << ' ' << (*it)->GetID() << std::endl;
		}

		return ss.str();
	}

	return std::string();
}
