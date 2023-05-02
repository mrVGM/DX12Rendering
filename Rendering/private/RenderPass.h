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
		virtual void Prepare() = 0;
		virtual void Execute() = 0;

		RenderPass(const BaseObjectMeta& meta);
		virtual ~RenderPass();
	};
}