#pragma once

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
		VariationNumber() = delete;
		VariationNumber(const std::vector<int>& digitLimits);
		VariationNumber(const int* digitLimits, int count);
		
		std::vector<int>& GetNumber();
		const std::vector<int>& GetDigits() const;
		bool SetNumber(long long num);
		long long GetIntegerRepresentation() const;

		bool Increment();

		long long GetMaxNumber() const;
	};
}
