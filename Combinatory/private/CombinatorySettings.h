#pragma once

#include "SettingsReader.h"

#include "BaseObjectMetaTag.h"

#include <string>
#include <list>
#include <vector>

namespace combinatory
{
	class CombinatorySettings : public settings::SettingsReader
	{
	public:
		struct Item
		{
			int m_width = 0;
			int m_length = 0;
			int m_count = 0;
			int m_id = -1;
		};

		struct Settings
		{
			int m_width = 0;
			std::list<Item> m_items;
		};
	private:
		Settings m_settings;

		void LoadSettings();
	public:
		std::vector<Item*> m_itemsSorted;

		CombinatorySettings();
		virtual ~CombinatorySettings();

		Settings& GetSettings();
	};
}
