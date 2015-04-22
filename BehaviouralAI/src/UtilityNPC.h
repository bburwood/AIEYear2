#ifndef UTILITYNPC_
#define UTILITYNPC_

#include "BaseNPC.h"
#include "UtilityScore.h"
#include "UtilityValue.h"
#include "World.h"

#include <map>

namespace UtilitySystem
{
	class UtilityNPC : public BaseNPC
	{
	public:
		UtilityNPC(World* pWorld);
		~UtilityNPC();
	protected:
		void selectAction(float a_fdeltaTime) override;
	private:
		UtilityValue	m_waterValue;
		UtilityValue	m_foodValue;
		UtilityValue	m_logValue;
		UtilityValue	m_restedValue;
		std::map<std::string, UtilityScore*> m_pUtilityScoreMap;
	};
}

#endif	//	UTILITYNPC_
