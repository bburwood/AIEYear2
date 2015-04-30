#include "BaseNPC.h"

#include <iostream>

#include "Gizmos.h"
#include "World.h"

#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>

BaseNPC::BaseNPC(World* a_pWorld)
{
	m_pWorld = a_pWorld;
	m_vPosition = glm::vec3(0, 0, 0);

	m_fMoveSpeed = 200.0f;

	m_bAlive = true;

	m_fLastReportTime = 0.0f;
	m_fReportTime = 1.0f;

	m_uiFood = 30;
	m_uiWater = 30;
	m_uiRested = 30;

	m_uiNumberOfLogs = 0;


	m_fLastFoodReductionTime = 0.0f;
	m_fLastWaterReductionTime = 0.0f;
	m_fLastRestedReductionTime = 0.0f;

	m_fFoodReductionTime = 2.0f;
	m_fWaterReductionTime = 2.5f;
	m_fRestedReductionTime = 3.0f;
}

BaseNPC::~BaseNPC()
{

}

void BaseNPC::update(float a_fdeltaTime)
{
	if (m_bAlive)
	{
		selectAction(a_fdeltaTime);
	
		calculateStatusChange();
		reportStatus();

		checkAlive();
	}
}

void BaseNPC::render(unsigned int a_uiIndex, unsigned int a_uiNumNPCs)
{
	if (m_bAlive)
	{
		Gizmos::addAABBFilled(m_vPosition + glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.5f, 2.0f, 0.5f), glm::vec4(0.2f, 0.25f, (float)a_uiIndex / (float)(a_uiNumNPCs - 1), 1.0f));
	}
	else
	{
		Gizmos::addAABBFilled(m_vPosition + glm::vec3(0, -2, 0), glm::vec3(0.5, 2, 0.5), glm::vec4(0.1, 0.1, 0.1, 1));
	}
}

void BaseNPC::collectWater(float a_fdeltaTime)
{
	if (travelTo(m_pWorld->getWaterLocation(), a_fdeltaTime))
	{
		if (m_pWorld->interactWithWater())
		{
			m_uiWater += 20;
			//std::cout << "Collected Water!" << std::endl;
		}
	}
}

void BaseNPC::collectFood(float a_fdeltaTime)
{
	if (travelTo(m_pWorld->getFoodLocation(), a_fdeltaTime))
	{
		if (m_pWorld->interactWithFood())
		{
			m_uiFood += 20;
			//std::cout << "Collected Food!" << std::endl;
		}
	}
}

void BaseNPC::rest(float a_fdeltaTime)
{
	if (travelTo(m_pWorld->getRestedLocation(), a_fdeltaTime))
	{
		if (m_pWorld->interactWithRested())
		{
			m_uiRested += 20;
			//std::cout << "Resting!" << std::endl;
		}
	}
}

void BaseNPC::chopTree(float a_fdeltaTime)
{
	if (travelTo(m_pWorld->getTreeLocation(), a_fdeltaTime))
	{
		if (m_pWorld->interactWithTree())
		{
			m_uiNumberOfLogs++;
			//std::cout << "Collected Log!" << std::endl;
		}
	}
}

void BaseNPC::buildHouse(unsigned int a_uiX, unsigned int a_uiZ, float a_fdeltaTime)
{
	if (travelTo(m_pWorld->getHouseLocation(a_uiX, a_uiZ), a_fdeltaTime))
	{
		if (m_uiNumberOfLogs == 0)
		{
			//std::cout << "Don't have any logs to build with :(" << std::endl;
		}
		else
		{
			if (m_pWorld->interactWithHouse(a_uiX, a_uiZ))
			{
				m_uiNumberOfLogs--;
				//std::cout << "Built House!" << std::endl;
				m_pWorld->addLogToHouse(a_uiX, a_uiZ);
			}
		}
	}
}

bool BaseNPC::travelTo(glm::vec3 a_vLoc, float a_fDeltaTime)
{
	glm::vec3 dirTo = (a_vLoc - m_vPosition);
	float distToPosition = glm::length(dirTo);
	dirTo = glm::normalize(dirTo);

	if (distToPosition <= 0.5)
		return true;
	else
	{
		float fMoveDist = a_fDeltaTime * m_fMoveSpeed;
		if (distToPosition < fMoveDist)
		{
			fMoveDist = distToPosition;
		}

		m_vPosition += dirTo * fMoveDist;
		return false;
	}
	
}

void BaseNPC::checkAlive()
{
	if (m_uiFood == 0)
	{
		std::cout << "Agent has starved to death!" << std::endl;
		m_bAlive = false;
	}

	if (m_uiWater == 0)
	{
		std::cout << "Agent has dehydrated!" << std::endl;
		m_bAlive = false;
	}

	if (m_uiRested == 0)
	{
		std::cout << "Agent has died due to exhaustion!!" << std::endl;
		m_bAlive = false;
	}
}

void BaseNPC::reportStatus()
{
	float fCurrentTime = (float)glfwGetTime();
	if (fCurrentTime >= m_fLastReportTime + m_fReportTime)
	{
		m_fLastReportTime = fCurrentTime;
		std::cout << "{ Food: " << m_uiFood;
		std::cout << ", Water: " << m_uiWater;
		std::cout << ", Rest: " << m_uiRested;
		std::cout << ", Logs: " << m_uiNumberOfLogs;
		std::cout << " }" << std::endl;
	}
}

void BaseNPC::calculateStatusChange()
{
	float fCurrentTime = (float)glfwGetTime();
	if (fCurrentTime >= m_fLastFoodReductionTime + m_fFoodReductionTime)
	{
		m_fLastFoodReductionTime = fCurrentTime;
		m_uiFood--;
	}
	if (fCurrentTime >= m_fLastWaterReductionTime + m_fWaterReductionTime)
	{
		m_fLastWaterReductionTime = fCurrentTime;
		m_uiWater--;
	}
	if (fCurrentTime >= m_fLastRestedReductionTime + m_fRestedReductionTime)
	{
		m_fLastRestedReductionTime = fCurrentTime;
		m_uiRested--;
	}
}
