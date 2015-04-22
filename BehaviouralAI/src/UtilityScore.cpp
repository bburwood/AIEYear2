#include "UtilityScore.h"
#include "UtilityValue.h"
namespace UtilitySystem
{
	UtilityScore::UtilityScore()
	{

	}

	UtilityScore::~UtilityScore()
	{

	}

	void UtilityScore::addUtilityValue(UtilityValue* a_pValue, float a_fModifier)
	{
		m_vUtilityValues.push_back({a_pValue, a_fModifier});
	}

	float UtilityScore::getUtilityScore()
	{
		float fScore = 0.0f;
		for (auto info : m_vUtilityValues)
		{
			if (fScore > 0)
			{
				fScore *= (info.pValue->evaluate() * info.fModifier);
			}
			else
			{
				fScore = (info.pValue->evaluate() * info.fModifier);
			}
		}

		return fScore;
	}
}