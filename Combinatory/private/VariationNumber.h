#pragma once

#include "CombinatorySettings.h"

#include <string>
#include <vector>

namespace combinatory
{
	class VariationNumber
	{
	private:
		std::vector<int> m_digitLimits;
		std::vector<int> m_number;

		void Init(const std::vector<int>& digitLimits);

	public:
		VariationNumber(const std::vector<int>& digitLimits);
		VariationNumber(const int* digitLimits, int count);
		VariationNumber(const VariationNumber& other) = delete;
		VariationNumber& operator=(const VariationNumber& other) = delete;

		const std::vector<int>& GetNumber() const;
		bool SetNumber(long long num);
		long long GetIntegerRepresentation() const;

		bool Increment();

		long long GetMaxNumber() const;
	};
}
