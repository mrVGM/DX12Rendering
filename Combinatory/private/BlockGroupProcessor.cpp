#include "BlockGroupProcessor.h"

namespace
{
	std::vector<int> GetGroupNumberTemplate(combinatory::BlockGroup* blockGroup)
	{
		std::vector<int> res;
		for (int i = 0; i < blockGroup->m_blocksOrdered.size(); ++i)
		{
			int digit = blockGroup->m_blocksOrdered[i]->m_maxCount;
			res.push_back(digit);
		}

		return res;
	}

	std::vector<int> GetGroupProcessorTemplate(combinatory::BlockGroup* blockGroup)
	{
		std::vector<int> res;
		for (int i = 0; i < blockGroup->m_blocksOrdered.size(); ++i)
		{
			res.push_back(1);
		}

		return res;
	}
}

combinatory::BlockGroupProcessor::BlockGroupProcessor(BlockGroup* blockGroup, int processorTemplateID) :
	m_blockGroup(blockGroup),
	m_groupNumber(GetGroupNumberTemplate(blockGroup)),
	m_bestGroupNumber(GetGroupNumberTemplate(blockGroup)),
	m_processorTemplate(GetGroupProcessorTemplate(blockGroup))
{
	m_processorTemplate.SetNumber(processorTemplateID);

	const std::vector<int> processorTemplate = m_processorTemplate.GetNumber();

	std::vector<int>& num = m_groupNumber.GetNumber();
	std::vector<int>& bestNum = m_bestGroupNumber.GetNumber();

	for (int i = 0; i < num.size(); ++i)
	{
		while (num[i] % 2 != processorTemplate[i])
		{
			++num[i];
			++bestNum[i];
		}
	}
}

void combinatory::BlockGroupProcessor::SetProcessorTemplate(int number)
{
	m_processorTemplate.SetNumber(number);
}

bool combinatory::BlockGroupProcessor::IncrementCurGroupNumber()
{
	const std::vector<int>& digits = m_groupNumber.GetDigits();
	std::vector<int>& curNum = m_groupNumber.GetNumber();

	const std::vector<int>& processorNum = m_processorTemplate.GetNumber();

	for (int i = curNum.size() - 1; i >= 0; --i)
	{
		int limit = digits[i];
		int curDigit = curNum[i];

		curDigit += 2;

		if (curDigit > limit)
		{
			continue;
		}

		curNum[i] = curDigit;
		for (int j = i + 1; j < curNum.size(); ++j)
		{
			curNum[j] = 0;
			while (curNum[j] % 2 != processorNum[j])
			{
				++curNum[j];
			}
		}
		return true;
	}

	return false;
}

