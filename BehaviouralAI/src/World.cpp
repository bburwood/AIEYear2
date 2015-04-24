#include "World.h"
#include "Gizmos.h"

#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>

#include <iostream>

World::World()
{
	//	initialise the houses
	m_houseBaseLocation = glm::vec3(4, 0, -4);
	m_houseSpacing = glm::vec3(3.0f, 0.0f, -3.0f);
	for (unsigned int x = 0; x < cg_uiNUM_HOUSESX; ++x)
	{
		float fCurrentX = x * m_houseSpacing.x;
		for (unsigned int z = 0; z < cg_uiNUM_HOUSESZ; ++z)
		{
			oHouseArray[x][z].m_houseLocation = m_houseBaseLocation + glm::vec3(fCurrentX, 0.0f, z * m_houseSpacing.z);
			oHouseArray[x][z].m_uiHouseLogsRequired = 250;
			oHouseArray[x][z].m_uiHouseCurrentLogs = 1;
			oHouseArray[x][z].m_fHouseInteractTime = 0.7f;
			oHouseArray[x][z].m_fLastHouseInteractTime = 0.0f;
		}
	}

	m_treeLocation = glm::vec3(-10, 0, -10);

	m_foodLocation = glm::vec3(10, 0, 10);
	m_waterLocation = glm::vec3(0, 0, 10);
	m_restedLocation = glm::vec3(-10, 0, 10);

	m_uiHouseCurrentLogs = 1;
	m_uiHouseLogsRequired = 500;

	m_fRestedInteractTime = 0.05f;
	m_fWaterInteractTime = 0.01f;
	m_fFoodInteractTime =  0.01f;

	m_fHouseInteractTime = 0.2;
	m_fTreeInteractTime = 0.002f;

	m_fLastFoodInteractTime = 0.0f;
	m_fLastWaterInteractTime = 0.0f;
	m_fLastRestedInteractTime = 0.0f;
	m_fLastHouseInteractTime = 0.0f;
	m_fLastTreeInteractTime = 0.0f;
}

World::~World()
{

}

void World::render()
{
	Gizmos::addSphere(m_foodLocation, 1, 8, 8, glm::vec4(1, 0, 0, 1));
	Gizmos::addSphere(m_waterLocation, 1, 8, 8, glm::vec4(0, 0, 1, 1));
	Gizmos::addSphere(m_restedLocation, 1, 8, 8, glm::vec4(0, 1, 1, 1));

	for (unsigned int x = 0; x < cg_uiNUM_HOUSESX; ++x)
	{
		for (unsigned int z = 0; z < cg_uiNUM_HOUSESZ; ++z)
		{
			float m_fHouseHeight = 1.0f * ((float)oHouseArray[x][z].m_uiHouseCurrentLogs / oHouseArray[x][z].m_uiHouseLogsRequired);
			//glm::vec4 houseColor = (oHouseArray[x][z].m_uiHouseCurrentLogs >= oHouseArray[x][z].m_uiHouseLogsRequired) ? glm::vec4(0.0f, 0.7f, 0.0f, 1.0f) : glm::vec4(1, 1, 0, 1);
			glm::vec4 houseColor = glm::vec4((float)x / (float)cg_uiNUM_HOUSESX, (float)z / cg_uiNUM_HOUSESZ, (float)oHouseArray[x][z].m_uiHouseCurrentLogs / (float)m_uiHouseLogsRequired, 1);

			Gizmos::addAABBFilled(oHouseArray[x][z].m_houseLocation + glm::vec3(0, m_fHouseHeight, 0), glm::vec3(1, m_fHouseHeight, 1), houseColor);
		}
	}

	Gizmos::addCylinderFilled(m_treeLocation, 1, 2, 8, glm::vec4(0, 1, 0, 1));
}

void World::addLogToHouse(unsigned int a_uiX, unsigned int a_uiZ)
{
//	if (m_uiHouseCurrentLogs < m_uiHouseLogsRequired)
	oHouseArray[a_uiX][a_uiZ].m_uiHouseCurrentLogs++;
}

bool World::interactWithFood()
{
	float fCurrentTime = (float)glfwGetTime();
	if (fCurrentTime >= m_fLastFoodInteractTime + m_fFoodInteractTime)
	{
		m_fLastFoodInteractTime = fCurrentTime;
		return true;
	}
	return false;
}

bool World::interactWithWater()
{
	float fCurrentTime = (float)glfwGetTime();

	if (fCurrentTime >= m_fLastWaterInteractTime + m_fWaterInteractTime)
	{
		m_fLastWaterInteractTime = fCurrentTime;
		return true;
	}

	return false;
}

bool World::interactWithRested()
{
	float fCurrentTime = (float)glfwGetTime();
	if (fCurrentTime >= m_fLastRestedInteractTime + m_fRestedInteractTime)
	{
		m_fLastRestedInteractTime = fCurrentTime;
		return true;
	}
	return false;
}

bool World::interactWithTree()
{
	float fCurrentTime = (float)glfwGetTime();
	if (fCurrentTime >= m_fLastTreeInteractTime + m_fTreeInteractTime)
	{
		m_fLastTreeInteractTime = fCurrentTime;
		return true;
	}
	return false;
}

bool World::interactWithHouse(unsigned int a_uiX, unsigned int a_uiZ)
{
	float fCurrentTime = (float)glfwGetTime();
	if (fCurrentTime >= oHouseArray[a_uiX][a_uiZ].m_fLastHouseInteractTime + oHouseArray[a_uiX][a_uiZ].m_fHouseInteractTime)
	{
		oHouseArray[a_uiX][a_uiZ].m_fLastHouseInteractTime = fCurrentTime;
		return true;
	}
	return false;
}

glm::vec3 World::getRestedLocation() const
{
	return m_restedLocation;
}

glm::vec3 World::getFoodLocation() const
{
	return m_foodLocation;
}

glm::vec3 World::getWaterLocation() const
{
	return m_waterLocation;
}

glm::vec3 World::getTreeLocation() const
{
	return m_treeLocation;
}

glm::vec3 World::getHouseLocation(unsigned int a_uiX, unsigned int a_uiZ) const
{
	return oHouseArray[a_uiX][a_uiZ].m_houseLocation;
}
