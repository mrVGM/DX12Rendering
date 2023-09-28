#include "CommandReader.h"

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

	return std::string();
}
