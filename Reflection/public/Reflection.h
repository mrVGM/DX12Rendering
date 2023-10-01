#pragma once

#include <string>

namespace reflection
{
	class ObjectWithID
	{
	public:
		virtual const std::string& GetID() const = 0;
	};

	void Boot();

	void CreateTestStruct();
}