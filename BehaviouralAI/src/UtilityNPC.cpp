#include "UtilityNPC.h"
#include "World.h"
#include <iostream>

using namespace std;

namespace UtilitySystem
{
	UtilityNPC::UtilityNPC(World* pWorld) : BaseNPC(pWorld)
	{
		m_waterValue.setNormalizationType(UtilityValue::INVERSE_LINEAR);
		m_waterValue.setMinMaxValues(5, 25);
		m_waterValue.setValue(getWaterValue());

		m_foodValue.setNormalizationType(UtilityValue::INVERSE_LINEAR);
		m_foodValue.setMinMaxValues(5, 25);
		m_foodValue.setValue(getFoodValue());

		m_logValue.setNormalizationType(UtilityValue::INVERSE_LINEAR);
		m_logValue.setMinMaxValues(0, 20);
		m_logValue.setValue(getNumberOfLogs());

		m_restedValue.setNormalizationType(UtilityValue::INVERSE_LINEAR);
		m_restedValue.setMinMaxValues(5, 25);
		m_restedValue.setValue(getRestValue());


		UtilityScore* pWaterScore = new UtilityScore();
		pWaterScore->addUtilityValue(&m_waterValue, 1.0f);
		m_pUtilityScoreMap["collectWater"] = pWaterScore;

		UtilityScore* pFoodScore = new UtilityScore();
		pFoodScore->addUtilityValue(&m_foodValue, 1.0f);
		m_pUtilityScoreMap["collectFood"] = pFoodScore;

		UtilityScore* pLogScore = new UtilityScore();
		pLogScore->addUtilityValue(&m_logValue, 1.0f);
		m_pUtilityScoreMap["collectLogs"] = pLogScore;

		UtilityScore* pRestScore = new UtilityScore();
		pRestScore->addUtilityValue(&m_restedValue, 1.0f);
		m_pUtilityScoreMap["needRest"] = pRestScore;

		uiHouseX = rand() % cg_uiNUM_HOUSESX;
		uiHouseZ = rand() % cg_uiNUM_HOUSESZ;

		m_fTimer = 0.0f;
		m_bNeedMoreLogs = true;
	}
	UtilityNPC::~UtilityNPC()
	{

	}
	void UtilityNPC::selectAction(float a_fdeltaTime)
	{
		m_waterValue.setValue(getWaterValue());
		m_foodValue.setValue(getFoodValue());
		m_logValue.setValue(getNumberOfLogs());
		m_restedValue.setValue(getRestValue());
		float fBestScore = 0.0f;
		m_fTimer += a_fdeltaTime;
		std::string strBestAction;
		for (auto score : m_pUtilityScoreMap)
		{
			float fThisScore = score.second->getUtilityScore();
			//if (m_fTimer > 1.0f)
			//{
			//	printf("Utility scores: %s: %0.3f\tfThisScore: %0.3f\n", score.first.c_str(), score.second->getUtilityScore(), fThisScore);
			//}
			if (fThisScore > fBestScore)
			{
				fBestScore = fThisScore;
				strBestAction = score.first;
				//if (m_fTimer > 1.0f)
				//{
				//	printf("Updated best score! New fBestScore: %0.3f\tNew best action: %s\n", fBestScore, strBestAction.c_str());
				//}
			}
		}
		if (m_fTimer > 1.0f)
		{
			m_fTimer = 0.0f;
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
			if (m_bNeedMoreLogs)
			{
				chopTree(a_fdeltaTime);
				if (getNumberOfLogs() > 25)
				{
					m_bNeedMoreLogs = false;
					//	pick a new house!
					uiHouseX = rand() % cg_uiNUM_HOUSESX;
					uiHouseZ = rand() % cg_uiNUM_HOUSESZ;
				}
			}
			else
			{
				buildHouse(uiHouseX, uiHouseZ, a_fdeltaTime);
				if (getNumberOfLogs() == 0)
				{
					m_bNeedMoreLogs = true;
				}
			}
		}
		else if (strBestAction == "needRest")
		{
			rest(a_fdeltaTime);
		}
		else
		{
			buildHouse(uiHouseX, uiHouseZ, a_fdeltaTime);
		}

	}
}