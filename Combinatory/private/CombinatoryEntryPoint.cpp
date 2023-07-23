#include "CombinatoryEntryPoint.h"

#include "CombinatoryEntryPointMeta.h"

#include "Combinatory.h"

#include "utils.h"

#include <vector>
#include <sstream>
#include <set>

namespace
{
	combinatory::CombinatorySettings* m_settings = nullptr;

	struct Block
	{
		std::vector<combinatory::CombinatorySettings::Item*> m_items;
		std::vector<int> m_quantities;
		int m_length;

		void CalcLengthAndQuantities()
		{
			int lcm = m_items[0]->m_length;
			for (int i = 1; i < m_items.size(); ++i)
			{
				lcm = combinatory::LCM(lcm, m_items[i]->m_length);
			}

			m_length = lcm;

			for (int i = 0; i < m_items.size(); ++i)
			{
				m_quantities.push_back(m_length / m_items[i]->m_length);
			}
		}
	};

	bool GenerateNextItemOrdering(std::vector<combinatory::CombinatorySettings::Item*>& ordering)
	{
		using namespace combinatory;

		for (int i = ordering.size() - 1; i >= 0; --i)
		{
			CombinatorySettings::Item*& cur = ordering[i];
			if (!cur)
			{
				cur = m_settings->m_itemsSorted[0];
				for (int j = i + 1; j < ordering.size(); ++j)
				{
					ordering[j] = nullptr;
				}
				return true;
			}

			if (cur->m_id >= m_settings->m_itemsSorted.size() - 1)
			{
				continue;
			}

			cur = m_settings->m_itemsSorted[cur->m_id + 1];
			for (int j = i + 1; j < ordering.size(); ++j)
			{
				ordering[j] = nullptr;
			}
			return true;
		}

		return false;
	}

	bool CheckOrdering(const std::vector<combinatory::CombinatorySettings::Item*>& ordering)
	{
		using namespace combinatory;

		int sum = 0;

		for (int i = 0; i < ordering.size(); ++i)
		{
			CombinatorySettings::Item* cur = ordering[i];

			if (cur)
			{
				sum += cur->m_width;
			}
		}

		if (sum == m_settings->GetSettings().m_width)
		{
			return true;
		}

		return false;
	}

	std::string EncodeOrdering(const std::vector<combinatory::CombinatorySettings::Item*>& ordering)
	{
		std::vector<int> itemsBlock;
		for (int i = 0; i < ordering.size(); ++i)
		{
			if (!ordering[i])
			{
				continue;
			}

			itemsBlock.push_back(ordering[i]->m_id);
		}

		for (int i = 0; i < itemsBlock.size() - 1; ++i)
		{
			for (int j = i; j < itemsBlock.size(); ++j)
			{
				if (itemsBlock[i] > itemsBlock[j])
				{
					int tmp = itemsBlock[i];
					itemsBlock[i] = itemsBlock[j];
					itemsBlock[j] = tmp;
				}
			}
		}

		std::stringstream ss;
		for (int i = 0; i < itemsBlock.size(); ++i)
		{
			ss << itemsBlock[i];
			if (i < itemsBlock.size() - 1)
			{
				ss << " ";
			}
		}

		return ss.str();
	}


}

combinatory::CombinatoryEntryPoint::CombinatoryEntryPoint() :
	settings::AppEntryPoint(CombinatoryEntryPointMeta::GetInstance())
{
}

combinatory::CombinatoryEntryPoint::~CombinatoryEntryPoint()
{
}

void combinatory::CombinatoryEntryPoint::Boot()
{
	combinatory::Boot();

	m_settings = combinatory::GetSettings();

	m_settings->GetSettings();

	std::vector<CombinatorySettings::Item*> ordering;
	int ordSize = m_settings->GetSettings().m_width / m_settings->m_itemsSorted[0]->m_width;

	for (int i = 0; i < ordSize; ++i)
	{
		ordering.push_back(nullptr);
	}

	int iterations = 0;
	int goodOrderings = 0;

	std::set<std::string> ords;

	while (GenerateNextItemOrdering(ordering))
	{
		++iterations;

		if (CheckOrdering(ordering))
		{
			++goodOrderings;
			ords.insert(EncodeOrdering(ordering));
		}
	}

	std::list<Block> blocks;
	for (auto it = ords.begin(); it != ords.end(); ++it)
	{
		std::stringstream ss(*it);
		Block block;

		while (!ss.eof())
		{
			int id;
			ss >> id;
			block.m_items.push_back(m_settings->m_itemsSorted[id]);
		}
		block.CalcLengthAndQuantities();

		blocks.push_back(block);
	}


	bool t = true;
}
