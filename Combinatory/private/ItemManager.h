#pragma once

#include "CombinatorySettings.h"

#include "Entities.h"

#include <string>
#include <list>
#include <vector>

namespace combinatory
{
	class ItemManager
	{
	private:
		std::vector<Item*> m_itemsSorted;
		std::vector<Block> m_blocks;

	public:
		ItemManager(CombinatorySettings& combinatorySettings);

		int GetItemsCount();
		Item* GetItemByID(int id);

		void Test();

		void GenerateBlocks();
	};
}
