#ifndef WORLD_H_
#define WORLD_H_

#include "glm/glm.hpp"

class World
{
public:
	World();
	~World();

	void render();

	void addLogToHouse();
	unsigned int getHouseLogsRequired() const { return m_uiHouseLogsRequired;  }
	unsigned int getCurrentHouseLogs() const { return m_uiHouseCurrentLogs; }
	bool isHouseBuilt() { return m_uiHouseCurrentLogs == m_uiHouseLogsRequired;  }

	glm::vec3 getHouseLocation() const;
	glm::vec3 getTreeLocation() const;
	glm::vec3 getWaterLocation() const;
	glm::vec3 getFoodLocation() const;
	glm::vec3 getRestedLocation() const;

	bool interactWithFood();
	bool interactWithWater();
	bool interactWithRested();

	bool interactWithTree();
	bool interactWithHouse();
private:
	unsigned int m_uiHouseLogsRequired;
	unsigned int m_uiHouseCurrentLogs;

	glm::vec3 m_houseLocation;
	glm::vec3 m_treeLocation;


	glm::vec3 m_waterLocation;
	glm::vec3 m_foodLocation;
	glm::vec3 m_restedLocation;

	float m_fHouseInteractTime;
	float m_fTreeInteractTime;
	float m_fFoodInteractTime;
	float m_fWaterInteractTime;
	float m_fRestedInteractTime;

	float m_fLastHouseInteractTime;
	float m_fLastTreeInteractTime;
	float m_fLastWaterInteractTime;
	float m_fLastFoodInteractTime;
	float m_fLastRestedInteractTime;
};

#endif // !WORLD_H_
