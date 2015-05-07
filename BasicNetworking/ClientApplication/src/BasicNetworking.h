#pragma once

#include "BaseApplication.h"
#include "../../ServerApplication/GameMessages.h"

namespace RakNet
{
	class RakPeerInterface;
}

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
	RakNet::RakPeerInterface* m_pPeerInterface;
	const char* IP = "127.0.0.1";
	const unsigned short PORT = 5456;};