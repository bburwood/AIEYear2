#include "World.h"
#include "Gizmos.h"

#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>

#include <iostream>

World::World()
{
	m_houseLocation = glm::vec3(10, 0, -10);
	m_treeLocation = glm::vec3(-10, 0, -10);

	m_foodLocation = glm::vec3(10, 0, 10);
	m_waterLocation = glm::vec3(0, 0, 10);
	m_restedLocation = glm::vec3(-10, 0, 10);

	m_uiHouseCurrentLogs = 4;
	m_uiHouseLogsRequired = 10;

	m_fRestedInteractTime = 1.0f;
	m_fWaterInteractTime = 2.0f;
	m_fFoodInteractTime =  2.0f;

	m_fHouseInteractTime = 3.0;
	m_fTreeInteractTime = 1.0f;

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


	float m_fHouseHeight = 3 * ((float)m_uiHouseCurrentLogs / m_uiHouseLogsRequired);
	glm::vec4 houseColor = (m_uiHouseCurrentLogs >= m_uiHouseLogsRequired) ? glm::vec4(0, 1, 0, 1) : glm::vec4(1, 1, 0, 1);

	Gizmos::addAABBFilled(m_houseLocation + glm::vec3(0, m_fHouseHeight, 0), glm::vec3(3, m_fHouseHeight, 2), houseColor);

	Gizmos::addCylinderFilled(m_treeLocation, 1, 2, 8, glm::vec4(0, 1, 0, 1));
}

void World::addLogToHouse()
{
	if (m_uiHouseCurrentLogs < m_uiHouseLogsRequired)
		m_uiHouseCurrentLogs++;
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

bool World::interactWithHouse()
{
	float fCurrentTime = (float)glfwGetTime();
	if (fCurrentTime >= m_fLastHouseInteractTime + m_fHouseInteractTime)
	{
		m_fLastHouseInteractTime = fCurrentTime;
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

glm::vec3 World::getHouseLocation() const
{
	return m_houseLocation;
}
