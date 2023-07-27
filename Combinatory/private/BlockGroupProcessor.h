#pragma once

#include "VariationNumber.h"
#include "Entities.h"
#include "Job.h"

#include <string>
#include <vector>
#include <list>

namespace combinatory
{
	class BlockGroupProcessor
	{
		friend class BlockGroupProcessorManager;
		friend class ProcessSomeNumbers;
		friend class SyncResults;
	private:
		BlockGroupProcessorManager* m_manager = nullptr;
		BlockGroup* m_blockGroup = nullptr;

		VariationNumber m_groupNumber;
		VariationNumber m_bestGroupNumber;
		VariationNumber m_processorTemplate;

		int m_bestScore = -1;
	public:
		BlockGroupProcessor(BlockGroupProcessorManager* manager, BlockGroup* blockGroup, const VariationNumber& processorTemplate);

		bool IncrementCurGroupNumber();

		void StartProcessing();
		void StoreProcessorResult();

		double GetProgress();
		int GetBest();

		BlockGroupProcessorManager* GetManager();
	};

	struct BlockGroupProcessorManager
	{
		BlockGroup* m_blockGroup = nullptr;
		std::vector<BlockGroupProcessor*> m_processors;

		VariationNumber m_bestNumber;
		int m_bestScore = -1;

		BlockGroupProcessorManager(BlockGroup* blockGroup);
		~BlockGroupProcessorManager();

		void StartProcessing();

		int GetScore(VariationNumber& vn);

		std::string GetDiffByNumber(VariationNumber& num);
	};

	struct JobCtx
	{
		BlockGroupProcessor* m_blockGroupProcessor = nullptr;
		bool m_done = false;
	};

	class SyncResults : public jobs::Job
	{
	private:
		JobCtx m_ctx;
	public:
		SyncResults(const JobCtx& ctx);
		void Do() override;
	};


	class ProcessSomeNumbers : public jobs::Job
	{
	private:
		JobCtx m_ctx;
	public:
		ProcessSomeNumbers(const JobCtx& ctx);
		void Do() override;
	};
}
