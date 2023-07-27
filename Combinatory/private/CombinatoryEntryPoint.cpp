#include "CombinatoryEntryPoint.h"

#include "CombinatoryEntryPointMeta.h"

#include "Combinatory.h"

#include "ItemManager.h"

#include "utils.h"

#include <vector>
#include <sstream>
#include <set>

namespace
{
	combinatory::CombinatorySettings* m_settings = nullptr;

	bool GenerateNextItemOrdering(std::vector<combinatory::Item*>& ordering)
	{
		using namespace combinatory;

		for (int i = ordering.size() - 1; i >= 0; --i)
		{
			Item*& cur = ordering[i];
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

	bool CheckOrdering(const std::vector<combinatory::Item*>& ordering)
	{
		using namespace combinatory;

		int sum = 0;

		for (int i = 0; i < ordering.size(); ++i)
		{
			Item* cur = ordering[i];

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

	std::string EncodeOrdering(const std::vector<combinatory::Item*>& ordering)
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

	ItemManager* itemManager = new ItemManager(*m_settings);
	itemManager->GenerateBlocks();
	itemManager->SeparateBlocksInGroups();

	itemManager->StartSolvingBlockGroups();
}
