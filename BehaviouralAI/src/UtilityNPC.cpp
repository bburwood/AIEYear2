#include "UtilityNPC.h"
#include "World.h"

namespace UtilitySystem
{
	UtilityNPC::UtilityNPC(World* pWorld) : BaseNPC(pWorld)
	{
		m_waterValue.setNormalizationType(UtilityValue::INVERSE_LINEAR);
		m_waterValue.setMinMaxValues(0, 20);
		m_waterValue.setValue(getWaterValue());

		m_foodValue.setNormalizationType(UtilityValue::INVERSE_LINEAR);
		m_foodValue.setMinMaxValues(0, 20);
		m_foodValue.setValue(getFoodValue());

		m_logValue.setNormalizationType(UtilityValue::INVERSE_LINEAR);
		m_logValue.setMinMaxValues(0, 20);
		m_logValue.setValue(getNumberOfLogs());

		m_restedValue.setNormalizationType(UtilityValue::INVERSE_LINEAR);
		m_restedValue.setMinMaxValues(0, 20);
		m_restedValue.setValue(getRestValue());


		UtilityScore* pWaterScore = new UtilityScore();
		pWaterScore->addUtilityValue(&m_waterValue, 1.0f);
		m_pUtilityScoreMap["collectWater"] = pWaterScore;

		UtilityScore* pFoodScore = new UtilityScore();
		pFoodScore->addUtilityValue(&m_foodValue, 1.0f);
		m_pUtilityScoreMap["collectFood"] = pFoodScore;

		UtilityScore* pLogScore = new UtilityScore();
		pLogScore->addUtilityValue(&m_waterValue, 1.0f);
		m_pUtilityScoreMap["collectLogs"] = pLogScore;

		UtilityScore* pRestScore = new UtilityScore();
		pRestScore->addUtilityValue(&m_waterValue, 1.0f);
		m_pUtilityScoreMap["needRest"] = pRestScore;
	}
	UtilityNPC::~UtilityNPC()
	{

	}
	void UtilityNPC::selectAction(float a_fdeltaTime)
	{
		m_waterValue.setValue(getWaterValue());
		float fBestScore = 0.0f;
		std::string strBestAction;
		for (auto score : m_pUtilityScoreMap)
		{
			float fThisScore = score.second->getUtilityScore();
			if (fThisScore > fBestScore)
			{
				fBestScore = fThisScore;
				strBestAction = score.first;
			}
		}
		if (strBestAction == "collectWater")
		{
			collectWater(a_fdeltaTime);
		}
		else if (strBestAction == "collectFood")
		{
			collectFood(a_fdeltaTime);
		}
		else if (strBestAction == "collectLogs")
		{
			chopTree(a_fdeltaTime);
		}
		else if (strBestAction == "needRest")
		{
			rest(a_fdeltaTime);
		}
		else
		{
			buildHouse(a_fdeltaTime);
		}

	}



}