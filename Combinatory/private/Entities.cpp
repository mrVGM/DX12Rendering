#include "Entities.h"

#include "utils.h"

#include <sstream>

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
