#pragma once

#include "SettingsReader.h"

#include "BaseObjectMetaTag.h"

#include <string>
#include <list>
#include <vector>

namespace combinatory
{
	struct Item
	{
		int m_width = 0;
		int m_length = 0;
		int m_count = 0;
		int m_id = -1;
	};

	struct ItemGroup
	{
		Item* m_item = nullptr;
		int count = 0;
	};
	
	struct Block
	{
		std::vector<ItemGroup> m_items;
		int m_maxCount = 0;

		std::string GetBlockCode();
		void CalculateItemCounts();
		void CalculateBlockMaxCount();
	};
}
