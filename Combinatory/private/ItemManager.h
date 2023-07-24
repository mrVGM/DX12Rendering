#pragma once

#include "CombinatorySettings.h"

#include <string>
#include <list>
#include <vector>

namespace combinatory
{
	class ItemManager
	{
	private:
		std::vector<Item*> m_itemsSorted;
	public:
		ItemManager(CombinatorySettings& combinatorySettings);

		int GetItemsCount();
		Item* GetItemByID(int id);
	};
}
