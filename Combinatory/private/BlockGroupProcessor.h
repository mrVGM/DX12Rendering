#pragma once

#include "VariationNumber.h"
#include "Entities.h"

#include <string>
#include <vector>

namespace combinatory
{
	class BlockGroupProcessor
	{
	private:
		BlockGroup* m_blockGroup = nullptr;

		VariationNumber m_groupNumber;
		VariationNumber m_bestGroupNumber;
		VariationNumber m_processorTemplate;

		int m_bestScore = -1;

	public:
		BlockGroupProcessor(BlockGroup* blockGroup, int processorTemplateID);

		void SetProcessorTemplate(int number);
		bool IncrementCurGroupNumber();
	};
}
