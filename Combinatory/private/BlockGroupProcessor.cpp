#include "BlockGroupProcessor.h"

#include "utils.h"

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

combinatory::BlockGroupProcessor::BlockGroupProcessor(BlockGroupProcessorManager* manager, BlockGroup* blockGroup, int processorTemplateID) :
	m_manager(manager),
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

combinatory::ProcessSomeNumbers::ProcessSomeNumbers(const JobCtx& ctx) :
	m_ctx(ctx)
{
}

void combinatory::ProcessSomeNumbers::Do()
{
	for (int i = 0; i < 300; ++i)
	{
		int assessment = m_ctx.m_blockGroupProcessor->m_blockGroup->AssessNumber(m_ctx.m_blockGroupProcessor->m_groupNumber);
		bool inc = m_ctx.m_blockGroupProcessor->IncrementCurGroupNumber();

		if (assessment >= 0)
		{
			if (m_ctx.m_blockGroupProcessor->m_bestScore < 0 || m_ctx.m_blockGroupProcessor->m_bestScore > assessment)
			{
				m_ctx.m_blockGroupProcessor->m_bestScore = assessment;
				m_ctx.m_blockGroupProcessor->m_bestGroupNumber.SetNumber(m_ctx.m_blockGroupProcessor->m_groupNumber.GetIntegerRepresentation());
			}
		}

		if (!inc)
		{
			m_ctx.m_done = true;
			break;
		}
	}

	RunSync(new SyncResults(m_ctx));

	class LogJob : public jobs::Job
	{
	private:
		BlockGroupProcessor* m_processor = nullptr;
	public:
		LogJob(BlockGroupProcessor* processor) :
			m_processor(processor)
		{
		}

		void Do() override
		{
			if (m_processor->GetManager()->m_bestProcessor)
			{
				std::cout << m_processor->GetManager()->m_bestProcessor->GetBest() << std::endl;
			}
		}
	};


	RunLog(new LogJob(m_ctx.m_blockGroupProcessor));
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
	if (!m_manager->m_bestProcessor)
	{
		m_manager->m_bestProcessor = this;
		return;
	}

	if (m_bestScore < 0)
	{
		return;
	}

	if (m_manager->m_bestProcessor->m_bestScore < 0 || m_manager->m_bestProcessor->m_bestScore > m_bestScore)
	{
		m_manager->m_bestProcessor = this;
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
	m_blockGroup(blockGroup)
{
	std::vector<int> tmp;
	for (int i = 0; i < m_blockGroup->m_blocksOrdered.size(); ++i)
	{
		tmp.push_back(1);
	}

	VariationNumber vn(tmp);
	long long maxNum = vn.GetMaxNumber();
	for (int i = 0; i <= maxNum; ++i)
	{
		BlockGroupProcessor* processor = new BlockGroupProcessor(this, m_blockGroup, i);
		m_processors.push_back(processor);
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
