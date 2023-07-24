#include "ItemManager.h"

#include "VariationNumber.h"

#include <map>

#include "utils.h"

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
		for (int j = i + 1; j < m_itemsSorted.size(); ++j)
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

void combinatory::ItemManager::Test()
{
	int vnTemplate[] = {2,3,4};
	VariationNumber vn(vnTemplate, _countof(vnTemplate));
	VariationNumber vn2(vnTemplate, _countof(vnTemplate));

	while (vn.Increment())
	{
		long long intRepr = vn.GetIntegerRepresentation();
		vn2.SetNumber(intRepr);

		bool t = true;
	}
}

void combinatory::ItemManager::GenerateBlocks()
{
	int minWidth = m_itemsSorted[0]->m_width;

	CombinatorySettings* cs = GetSettings();
	int bandwidth = cs->GetSettings().m_width;

	int maxItems = bandwidth / minWidth;
	++maxItems;

	std::vector<int> vnTemplate;
	for (int i = 0; i < maxItems; ++i)
	{
		vnTemplate.push_back(m_itemsSorted.size());
	}
	VariationNumber vn(vnTemplate);

	std::map<std::string, Block> blocks;

	while (true)
	{
		int sum = 0;

		const std::vector<int>& number = vn.GetNumber();

		for (int i = 0; i < number.size(); ++i)
		{
			if (number[i] > 0)
			{
				Item* curItem = m_itemsSorted[number[i] - 1];
				sum += curItem->m_width;
			}
		}

		if (sum == 400 || sum == 396)
		{
			Block block;
			for (int i = 0; i < number.size(); ++i)
			{
				if (number[i] > 0)
				{
					Item* curItem = m_itemsSorted[number[i] - 1];
					ItemGroup ig;
					ig.m_item = curItem;
					block.m_items.push_back(ig);
				}
			}

			std::string blockCode = block.GetBlockCode();
			blocks[blockCode] = block;
		}

		if (!vn.Increment())
		{
			break;
		}
	}

	for (auto it = blocks.begin(); it != blocks.end(); ++it)
	{
		m_blocks.push_back(it->second);
	}

	for (int i = 0; i < m_blocks.size(); ++i)
	{
		Block& cur = m_blocks[i];
		cur.CalculateItemCounts();
		cur.CalculateBlockMaxCount();
	}

	for (int i = 0; i < m_blocks.size() - 1; ++i)
	{
		for (int j = i + 1; j < m_blocks.size(); ++j)
		{
			if (m_blocks[i].m_maxCount < m_blocks[j].m_maxCount)
			{
				Block tmp = m_blocks[i];
				m_blocks[i] = m_blocks[j];
				m_blocks[j] = tmp;
			}
		}
	}

	std::vector<int> coefs;
	for (int i = 0; i < m_blocks.size(); ++i)
	{
		coefs.push_back(m_blocks[i].m_maxCount);
	}
	VariationNumber sizeTest(coefs);

	long long intRepr = sizeTest.GetMaxNumber();
}
