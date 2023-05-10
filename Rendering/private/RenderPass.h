#pragma once

#include "BaseObject.h"

#include "Job.h"

class BaseObjectMeta;

namespace rendering
{
	class RenderPass : public BaseObject
	{
	private:
	public:
		virtual void Prepare() = 0;
		virtual void Execute() = 0;
		virtual void Load(jobs::Job* done) = 0;

		RenderPass(const BaseObjectMeta& meta);
		virtual ~RenderPass();
	};
}