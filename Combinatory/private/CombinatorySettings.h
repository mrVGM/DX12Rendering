#pragma once

#include "SettingsReader.h"

#include "BaseObjectMetaTag.h"

#include <string>
#include <map>

namespace combinatory
{
	class CombinatorySettings : public settings::SettingsReader
	{
	public:
		struct Settings
		{
			int m_width;
		};
	private:
		Settings m_settings;

		void LoadSettings();
	public:
		CombinatorySettings();
		virtual ~CombinatorySettings();
	};
}
