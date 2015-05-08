#pragma once

#include <vector>

#include "../../ServerApplication/GameMessages.h"
#include "../../ServerApplication/GameObject.h"
#include "BaseApplication.h"
#include "RakPeerInterface.h"
#include "Camera.h"

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
	void handleNetworkMessages();	
private:
	Camera	m_camera;
	RakNet::RakPeerInterface* m_pPeerInterface;
	const char* IP = "127.0.0.1";
	const unsigned short PORT = 5456;
	unsigned int m_uiClientId = 0;	//	0 is no ID yet
	std::vector<GameObject>	m_gameobjects;
};
