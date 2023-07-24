#include "ItemManager.h"

combinatory::ItemManager::ItemManager(CombinatorySettings& combinatorySettings)
{
	CombinatorySettings::Settings& settings = combinatorySettings.GetSettings();

	for (auto it = settings.m_items.begin(); it != settings.m_items.end(); ++it)
	{
		Item& cur = *it;
		m_itemsSorted.push_back(&cur);
	}

	for (int i = 0; i < m_itemsSorted.size() - 1; ++i)
	{
		for (int j = i; j < m_itemsSorted.size(); ++j)
		{
			if (m_itemsSorted[i]->m_width > m_itemsSorted[j]->m_width)
			{
				Item* tmp = m_itemsSorted[i];
				m_itemsSorted[i] = m_itemsSorted[j];
				m_itemsSorted[j] = tmp;
			}
		}
	}
	for (int i = 0; i < m_itemsSorted.size(); ++i)
	{
		m_itemsSorted[i]->m_id = i;
	}
}

int combinatory::ItemManager::GetItemsCount()
{
	return m_itemsSorted.size();
}

combinatory::Item* combinatory::ItemManager::GetItemByID(int id)
{
	return m_itemsSorted[id];
}
