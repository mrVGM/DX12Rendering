#pragma once

#include <string>
#include <list>

namespace frontend
{
	class CommandReader
	{
	private:
		std::list<std::string> m_messageCache;

	public:
		std::string ProcessMessage(const std::string& message);
	};
}