#include "Entities.h"

#include "VariationNumber.h"

#include "utils.h"

#include "BlockGroupProcessor.h"

#include <sstream>
#include <map>

std::string combinatory::Block::GetBlockCode()
{
	for (int i = 0; i < m_items.size() - 1; ++i)
	{
		for (int j = i; j < m_items.size(); ++j)
		{
			if (m_items[i].m_item->m_id > m_items[j].m_item->m_id)
			{
				ItemGroup tmp = m_items[i];
				m_items[i] = m_items[j];
				m_items[j] = tmp;
			}
		}
	}

	std::stringstream ss;
	for (int i = 0; i < m_items.size(); ++i)
	{
		ss << m_items[i].m_item->m_id << ' ';
	}

	return ss.str();
}

void combinatory::Block::CalculateItemCounts()
{
	int lcm = m_items[0].m_item->m_length;
	
	for (int i = 1; i < m_items.size(); ++i)
	{
		ItemGroup& cur = m_items[i];
		lcm = LCM(lcm, cur.m_item->m_length);
	}

	for (int i = 0; i < m_items.size(); ++i)
	{
		m_items[i].count = lcm / m_items[i].m_item->m_length;
	}
}

void combinatory::Block::CalculateBlockMaxCount()
{
	std::map<Item*, int> itemsInBlock;

	for (int i = 0; i < m_items.size(); ++i)
	{
		ItemGroup& ig = m_items[i];
		auto it = itemsInBlock.find(ig.m_item);
		if (it == itemsInBlock.end())
		{
			itemsInBlock[ig.m_item] = ig.count;
		}
		else
		{
			itemsInBlock[ig.m_item] = ig.count + it->second;
		}
	}

	int minCnt = -1;
	for (auto it = itemsInBlock.begin(); it != itemsInBlock.end(); ++it)
	{
		int cnt = it->first->m_count / it->second;
		++cnt;

		if (minCnt < 0 || minCnt > cnt)
		{
			minCnt = cnt;
		}
	}

	while (true)
	{
		bool passedUpperLimit = false;
		for (auto it = itemsInBlock.begin(); it != itemsInBlock.end(); ++it)
		{
			if (minCnt * it->second - it->first->m_count >= 5)
			{
				passedUpperLimit = true;
				break;
			}
		}

		if (passedUpperLimit)
		{
			break;
		}
		++minCnt;
	}

	m_maxCount = minCnt;
}

bool combinatory::Block::ContainsItem(Item* item)
{
	for (int i = 0; i < m_items.size(); ++i)
	{
		ItemGroup& cur = m_items[i];

		if (cur.m_item == item)
		{
			return true;
		}
	}
	return false;
}

int combinatory::BlockGroup::ItemOccurences(Item* item)
{
	int occurrences = 0;
	for (auto it = m_blocks.begin(); it != m_blocks.end(); ++it)
	{
		Block* cur = *it;

		if (cur->ContainsItem(item))
		{
			++occurrences;
		}
	}
	return occurrences;
}

void combinatory::BlockGroup::GetAllItems(std::set<Item*>& items)
{
	for (auto blockIt = m_blocks.begin(); blockIt != m_blocks.end(); ++blockIt)
	{
		Block* curBlock = *blockIt;
		for (auto itemIt = curBlock->m_items.begin(); itemIt != curBlock->m_items.end(); ++itemIt)
		{
			ItemGroup& itemGroup = *itemIt;
			items.insert(itemGroup.m_item);
		}
	}
}

void combinatory::BlockGroup::ShrinkGroup()
{
	std::set<Item*> items;
	GetAllItems(items);

	std::vector<Block*> blocksSorted;
	for (auto it = m_blocks.begin(); it != m_blocks.end(); ++it)
	{
		blocksSorted.push_back(*it);
	}

	for (int i = 0; i < blocksSorted.size() - 1; ++i)
	{
		for (int j = i + 1; j < blocksSorted.size(); ++j)
		{
			if (blocksSorted[i]->m_maxCount > blocksSorted[j]->m_maxCount)
			{
				Block* tmp = blocksSorted[i];
				blocksSorted[i] = blocksSorted[j];
				blocksSorted[j] = tmp;
			}
		}
	}

	bool shrinked = true;
	while (shrinked)
	{
		shrinked = false;

		for (auto it = blocksSorted.begin(); it != blocksSorted.end(); ++it)
		{
			if (!m_blocks.contains(*it))
			{
				continue;
			}

			bool canRemove = true;
			Block* curBlock = *it;
			for (int i = 0; i < curBlock->m_items.size(); ++i)
			{
				ItemGroup& curItem = curBlock->m_items[i];
				int occurences = ItemOccurences(curItem.m_item);

				if (occurences <= 2)
				{
					canRemove = false;
				}
			}

			if (canRemove)
			{
				m_blocks.erase(curBlock);
				shrinked = true;
			}
		}
	}

	FlattenBlocks();
}

bool combinatory::BlockGroup::IsEquivalent(BlockGroup& other)
{
	std::set<Item*> group1Items;
	GetAllItems(group1Items);

	std::set<Item*> group2Items;
	other.GetAllItems(group2Items);

	for (auto it = group1Items.begin(); it != group1Items.end(); ++it)
	{
		if (!group2Items.contains(*it))
		{
			return false;
		}
	}

	for (auto it = group2Items.begin(); it != group2Items.end(); ++it)
	{
		if (!group1Items.contains(*it))
		{
			return false;
		}
	}

	return true;
}

void combinatory::BlockGroup::FlattenBlocks()
{
	for (auto it = m_blocks.begin(); it != m_blocks.end(); ++it)
	{
		m_blocksOrdered.push_back(*it);
	}
}

void combinatory::BlockGroup::CalculateBestNumber()
{
	m_processorManager = new BlockGroupProcessorManager(this);
	m_processorManager->StartProcessing();
}

int combinatory::BlockGroup::AssessNumber(VariationNumber& number)
{
	std::map<Item*, int> itemInstances;
	const std::vector<int>& coefs = number.GetNumber();

	for (int i = 0; i < m_blocksOrdered.size(); ++i)
	{
		Block* curBlock = m_blocksOrdered[i];
		int curCoef = coefs[i];

		for (auto it = curBlock->m_items.begin(); it != curBlock->m_items.end(); ++it)
		{
			ItemGroup& curGroup = *it;
			int num = curGroup.count * curCoef;

			auto record = itemInstances.find(curGroup.m_item);

			if (record == itemInstances.end())
			{
				itemInstances[curGroup.m_item] = num;
			}
			else
			{
				itemInstances[curGroup.m_item] += num;
			}
		}
	}

	int s = -1;
	for (auto it = itemInstances.begin(); it != itemInstances.end(); ++it)
	{
		if (it->first->m_count < it->second)
		{
			return -1;
		}

		int diff = it->first->m_count - it->second;
		if (s < 0)
		{
			s = diff;
		}
		else
		{
			s += diff;
		}
	}

	return s;
}

combinatory::BlockGroup::~BlockGroup()
{
	if (m_processorManager)
	{
		delete m_processorManager;
	}
}
