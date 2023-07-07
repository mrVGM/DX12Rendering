#pragma once

#include "BaseObjectMeta.h"

namespace settings
{
	class SettingsReaderMeta : public BaseObjectMeta
	{
	public:
		static const SettingsReaderMeta& GetInstance();
		SettingsReaderMeta();
	};
}