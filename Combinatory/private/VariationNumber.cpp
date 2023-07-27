#include "VariationNumber.h"

#include <sstream>

void combinatory::VariationNumber::Init(const std::vector<int>& digitLimits)
{
	m_digitLimits = digitLimits;

	for (int i = 0; i < m_digitLimits.size(); ++i)
	{
		m_number.push_back(0);
	}
}

combinatory::VariationNumber::VariationNumber()
{
}

combinatory::VariationNumber::VariationNumber(const std::vector<int>& digitLimits)
{
	Init(digitLimits);
}

combinatory::VariationNumber::VariationNumber(const int* digitLimits, int count)
{
	std::vector<int> tmp;
	for (int i = 0; i < count; ++i)
	{
		tmp.push_back(digitLimits[i]);
	}

	Init(tmp);
}

std::vector<int>& combinatory::VariationNumber::GetNumber()
{
	return m_number;
}

const std::vector<int>& combinatory::VariationNumber::GetDigits() const
{
	return m_digitLimits;
}

bool combinatory::VariationNumber::SetNumber(long long num)
{
	long long left = num;
	int curDigitIndex = m_number.size() - 1;

	for (int i = m_number.size() - 1; i >= 0; --i)
	{
		int reminder = left % (m_digitLimits[i] + 1);
		m_number[i] = reminder;

		left -= reminder;
		left /= m_digitLimits[i] + 1;
	}

	if (left > 0)
	{
		return false;
	}

	return true;
}

long long combinatory::VariationNumber::GetIntegerRepresentation() const
{
	long long num = 0;
	long long coef = 1;

	for (int i = m_number.size() - 1; i >= 0; --i)
	{
		int curDigit = m_number[i];
		num += curDigit * coef;

		coef *= m_digitLimits[i] + 1;
	}

	return num;
}

bool combinatory::VariationNumber::Increment()
{
	for (int i = m_number.size() - 1; i >= 0; --i)
	{
		int& cur = m_number[i];

		if (cur >= m_digitLimits[i])
		{
			continue;
		}

		++cur;
		for (int j = i + 1; j < m_number.size(); ++j)
		{
			m_number[j] = 0;
		}
		return true;
	}

	return false;
}

long long combinatory::VariationNumber::GetMaxNumber() const
{
	VariationNumber tmp(m_digitLimits);
	for (int i = 0; i < m_digitLimits.size(); ++i)
	{
		tmp.m_number[i] = m_digitLimits[i];
	}

	return tmp.GetIntegerRepresentation();
}

std::string combinatory::VariationNumber::ToString()
{
	std::stringstream ss;
	ss << '(';
	for (int i = 0; i < m_number.size(); ++i)
	{
		ss << m_number[i];
		if (i < m_number.size() - 1)
		{
			ss << ' ';
		}
	}

	ss << ')';

	return ss.str();
}
