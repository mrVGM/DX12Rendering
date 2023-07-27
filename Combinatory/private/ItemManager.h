#pragma once

#include "CombinatorySettings.h"

#include "Entities.h"

#include "BaseObject.h"

#include <string>
#include <list>
#include <vector>

namespace combinatory
{
	class ItemManager : public BaseObject
	{
	private:
		std::vector<Item*> m_itemsSorted;
		std::vector<Block> m_blocks;
		std::vector<BlockGroup> m_blockGroups;

	public:
		ItemManager(CombinatorySettings& combinatorySettings);
		virtual ~ItemManager();

		int GetItemsCount();
		Item* GetItemByID(int id);

		void GenerateBlocks();

		void GenerateBlockGroup(Item* initialItem, BlockGroup& blockGroup);
		void SeparateBlocksInGroups();
	};
}
