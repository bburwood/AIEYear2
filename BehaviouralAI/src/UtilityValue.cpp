#include "UtilityValue.h"
#include "UtilityMaths.h"

#include <assert.h>
#include<algorithm>

namespace UtilitySystem
{

	UtilitySystem::UtilityValue::UtilityValue()
		: UtilityValue(LINEAR, 0, 1)
	{

	}

	UtilitySystem::UtilityValue::UtilityValue(NormalizationType a_eType, float a_fMin, float a_fMax)
		: m_fMin(a_fMin)
		, m_fMax(a_fMax)
		, m_fValue(a_fMin)
		, m_fNormalizedValue(0)
		, m_eNormalizationType(a_eType)
	{

	}

	UtilityValue::~UtilityValue()
	{

	}

	void UtilityValue::setValue(float a_fValue)
	{
		m_fValue = a_fValue;
	}

	void UtilityValue::setQuadraticPower(float a_fPower)
	{
		m_fPower = a_fPower;
	}

	void UtilityValue::setNormalizationType(NormalizationType a_eType)
	{
		m_eNormalizationType = a_eType;
	}


	void UtilityValue::setMinMaxValues(float a_fMin, float a_fMax)
	{
		m_fMin = a_fMin;
		m_fMax = a_fMax;
	}

	float UtilityValue::evaluate()
	{
		switch (m_eNormalizationType)
		{
		case UtilityValue::LINEAR:
			m_fNormalizedValue = UtilityMath::LinearNormalise(m_fMin, m_fMax, m_fValue);
			break;
		case UtilityValue::INVERSE_LINEAR:
			m_fNormalizedValue = 1.0f - UtilityMath::LinearNormalise(m_fMin, m_fMax, m_fValue);
			break;
		case UtilityValue::QUADRATIC:
			m_fNormalizedValue = UtilityMath::QuadraticNormalise(m_fMin, m_fMax, m_fValue, m_fPower);
			break;
		case UtilityValue::INVERSE_QUADRATIC:
			m_fNormalizedValue = 1.0f - UtilityMath::QuadraticNormalise(m_fMin, m_fMax, m_fValue, m_fPower);
			break;
		default:
			assert(false && "No Utility Function Set");
			break;
		}

		m_fNormalizedValue = std::max(std::min(m_fNormalizedValue, 1.0f), 0.0f);
		return m_fNormalizedValue;
	}

}


