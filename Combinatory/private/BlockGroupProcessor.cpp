#include "BlockGroupProcessor.h"

#include "utils.h"

#include <map>
#include <iostream>

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

combinatory::BlockGroupProcessor::BlockGroupProcessor(BlockGroupProcessorManager* manager, BlockGroup* blockGroup, const VariationNumber& processorTemplate) :
	m_manager(manager),
	m_blockGroup(blockGroup),
	m_processorTemplate(processorTemplate),
	m_groupNumber(GetGroupNumberTemplate(blockGroup)),
	m_bestGroupNumber(GetGroupNumberTemplate(blockGroup))
{
	std::vector<int>& num = m_groupNumber.GetNumber();
	for (int i = 0; i < num.size(); ++i)
	{
		while (num[i] % 2 != m_processorTemplate.GetNumber()[i])
		{
			++num[i];
		}
	}
	m_bestGroupNumber = m_groupNumber;
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

combinatory::ProcessSomeNumbers::ProcessSomeNumbers(const JobCtx& ctx) :
	m_ctx(ctx)
{
}

void combinatory::ProcessSomeNumbers::Do()
{
	for (int i = 0; i < 300; ++i)
	{
		int score = m_ctx.m_blockGroupProcessor->GetManager()->GetScore(m_ctx.m_blockGroupProcessor->m_groupNumber);
		bool inc = m_ctx.m_blockGroupProcessor->IncrementCurGroupNumber();

		
		if (m_ctx.m_blockGroupProcessor->m_bestScore < score)
		{
			m_ctx.m_blockGroupProcessor->m_bestScore = score;
			m_ctx.m_blockGroupProcessor->m_bestGroupNumber = m_ctx.m_blockGroupProcessor->m_groupNumber;
		}

		if (!inc)
		{
			m_ctx.m_done = true;
			break;
		}
	}

	RunSync(new SyncResults(m_ctx));
}

combinatory::SyncResults::SyncResults(const JobCtx& ctx) :
	m_ctx(ctx)
{
}

void combinatory::SyncResults::Do()
{
	m_ctx.m_blockGroupProcessor->StoreProcessorResult();

	if (!m_ctx.m_done)
	{
		RunAsync(new ProcessSomeNumbers(m_ctx));
	}
}

void combinatory::BlockGroupProcessor::StartProcessing()
{
	JobCtx ctx;
	ctx.m_blockGroupProcessor = this;

	RunAsync(new ProcessSomeNumbers(ctx));
}

void combinatory::BlockGroupProcessor::StoreProcessorResult()
{
	if (m_manager->m_bestScore < m_bestScore)
	{
		m_manager->m_bestNumber = m_bestGroupNumber;
		m_manager->m_bestScore = m_bestScore;

		struct CTX
		{
			int m_score;
			VariationNumber m_number;
		};

		class LogState : public jobs::Job
		{
		private:
			CTX m_ctx;
		public:
			LogState(const CTX& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				std::cout << m_ctx.m_score << ":\t" << m_ctx.m_number.ToString() << std::endl;
			}
		};

		RunLog(new LogState(CTX{ m_manager->m_bestScore, m_manager->m_bestNumber }));
	}
}

double combinatory::BlockGroupProcessor::GetProgress()
{
	return m_groupNumber.GetIntegerRepresentation() / (double) m_groupNumber.GetMaxNumber();
}

int combinatory::BlockGroupProcessor::GetBest()
{
	return m_bestScore;
}

combinatory::BlockGroupProcessorManager* combinatory::BlockGroupProcessor::GetManager()
{
	return m_manager;
}

combinatory::BlockGroupProcessorManager::BlockGroupProcessorManager(BlockGroup* blockGroup) :
	m_blockGroup(blockGroup),
	m_bestNumber(GetGroupNumberTemplate(blockGroup))
{
	std::vector<int> tmp;
	for (int i = 0; i < m_blockGroup->m_blocksOrdered.size(); ++i)
	{
		tmp.push_back(1);
	}

	VariationNumber vn(tmp);
	bool inc = true;

	while (inc)
	{
		BlockGroupProcessor* processor = new BlockGroupProcessor(this, m_blockGroup, vn);
		m_processors.push_back(processor);
		inc = vn.Increment();
	}
}

combinatory::BlockGroupProcessorManager::~BlockGroupProcessorManager()
{
	for (int i = 0; i < m_processors.size(); ++i)
	{
		delete m_processors[i];
	}
}

void combinatory::BlockGroupProcessorManager::StartProcessing()
{
	for (int i = 0; i < m_processors.size(); ++i)
	{
		m_processors[i]->StartProcessing();
	}
}

int combinatory::BlockGroupProcessorManager::GetScore(VariationNumber& vn)
{
	const std::vector<int> num = vn.GetNumber();

	std::set<Item*> items;
	std::map<Item*, int> counts;
	m_blockGroup->GetAllItems(items);

	for (int i = 0; i < m_blockGroup->m_blocksOrdered.size(); ++i)
	{
		Block* curBlock = m_blockGroup->m_blocksOrdered[i];
		for (auto itemIt = curBlock->m_items.begin(); itemIt != curBlock->m_items.end(); ++itemIt)
		{
			ItemGroup& curItem = *itemIt;

			auto record = counts.find(curItem.m_item);
			int n = num[i] * curItem.count;
			if (record == counts.end())
			{
				counts[curItem.m_item] = n;
			}
			else
			{
				counts[curItem.m_item] = record->second + n;
			}
		}
	}

	int score = 0;
	for (auto it = counts.begin(); it != counts.end(); ++it)
	{
		if (it->first->m_count < it->second)
		{
			return -1;
		}

		score += it->second;
	}

	return score;
}
