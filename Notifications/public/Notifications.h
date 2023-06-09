#pragma once

#include "BaseObjectMeta.h"

namespace notifications
{
	void Boot();
	void Notify(const BaseObjectMeta& meta);
}