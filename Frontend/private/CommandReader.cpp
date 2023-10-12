#include "CommandReader.h"

#include "TypeManager.h"

#include <sstream>

std::string frontend::CommandReader::ProcessMessage(const std::string& message)
{
	reflection::TypeManager& typeManager = reflection::TypeManager::GetInstance();

	std::stringstream messageStream(message);
	std::string command;
	messageStream >> command;

	if (command == ":ping")
	{
		return ":ping";
	}

	if (command == ":quit")
	{
		return ":quit";
	}

	if (command == ":types")
	{
		std::list<const reflection::DataDef*> types;
		typeManager.GetTypes(reflection::ValueType::Int, types);

		std::stringstream ss;
		for (auto it = types.begin(); it != types.end(); ++it)
		{
			ss << (*it)->GetName() << ' ' << (*it)->GetID() << std::endl;
		}

		return ss.str();
	}

	if (command == ":get_generated")
	{
		std::list<const reflection::DataDef*> types;
		typeManager.GetTypes(reflection::ValueType::Struct, types);

		std::stringstream ss;
		ss << "[";

		bool fst = true;
		for (auto it = types.begin(); it != types.end(); ++it)
		{
			if (!fst)
			{
				ss << ',';
			}
			fst = false;
			ss << xml_writer::EncodeAsString((*it)->GetID());
		}

		ss << ']';

		return ss.str();
	}

	if (command == ":get_asset_info")
	{
		std::string id;
		messageStream >> id;

		std::stringstream ss;

		const reflection::DataDef* type = typeManager.GetType(id);
		type->GetName();
		ss << "{ \"name\": " << '\"' << type->GetName() << '\"' << "}";

		return ss.str();
	}

	return std::string();
}
