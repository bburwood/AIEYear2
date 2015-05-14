#pragma once

#include <vector>

#include "../../ServerApplication/GameMessages.h"
#include "../../ServerApplication/GameObject.h"
#include "BaseApplication.h"
#include "RakPeerInterface.h"
#include "Camera.h"
#include "gl_core_4_4.h"

class BasicNetworkingApplication : public BaseApplication
{
public:
	BasicNetworkingApplication();
	~BasicNetworkingApplication();

	virtual bool startup();

	virtual void shutdown();

	virtual bool update(float deltaTime);

	virtual void draw();

	//Initialize the connection
	void handleNetworkConnection();
	void initialiseClientConnection();
	//Handle incoming packets
	void handleNetworkMessages();	void readObjectDataFromServer(RakNet::BitStream& bsIn);	void createGameObject();	void moveClientObject(float deltaTime);	void sendUpdatedObjectPositionToServer(GameObject& myClientObject);	
private:
	Camera	m_camera;
	RakNet::RakPeerInterface* m_pPeerInterface;
	const char* IP = "127.0.0.1";
	const unsigned short PORT = 5456;
	unsigned int m_uiClientId = 0;	//	0 is no ID yet
	unsigned int m_uiclientObjectIndex = 0;
	std::vector<GameObject>	m_gameObjects;
	glm::vec4	m_myColour;
	float	m_fSendTimer;
	float	m_fSendInterval = 0.10f;	//	send object position update data to server only after this time
	bool	m_bObjectMoved = false;
};
