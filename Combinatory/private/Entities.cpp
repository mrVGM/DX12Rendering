#include "Entities.h"

#include "utils.h"

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

		if (minCnt < cnt)
		{
			minCnt = cnt;
		}
	}

	m_maxCount = minCnt;
}
