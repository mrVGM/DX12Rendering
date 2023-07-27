#pragma once

#include "SettingsReader.h"

#include "BaseObjectMetaTag.h"

#include "VariationNumber.h"

#include <string>
#include <list>
#include <vector>
#include <set>

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
		bool ContainsItem(Item* item);
	};

	class BlockGroupProcessorManager;

	struct BlockGroup
	{
		std::set<Block*> m_blocks;
		std::vector<Block*> m_blocksOrdered;

		BlockGroupProcessorManager* m_processorManager = nullptr;

		int ItemOccurences(Item* item);
		void GetAllItems(std::set<Item*>& items);
		void ShrinkGroup();
		bool IsEquivalent(BlockGroup& other);

		void FlattenBlocks();
		void CalculateBestNumber();
		int AssessNumber(VariationNumber& number);

		~BlockGroup();
	};
}
