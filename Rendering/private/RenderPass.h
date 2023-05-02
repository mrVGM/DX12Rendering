#pragma once

#include "BaseObject.h"

#include <string>

class BaseObjectMeta;

namespace rendering
{
	class RenderPass : public BaseObject
	{
	private:
	public:
		virtual bool Prepare(std::string& error) = 0;
		virtual bool Execute(std::string& error) = 0;

		RenderPass(const BaseObjectMeta& meta);
		virtual ~RenderPass();
	};
}