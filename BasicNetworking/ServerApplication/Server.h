#pragma once
#include <iostream>
#include <string>

#include <thread>
#include <chrono>
#include <unordered_map>


#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"

#include "GameMessages.h"
#include "GameObject.h"


class Server
{
public:
	Server();
	~Server();

	void run();
protected:

	void handleNetworkMessages();
	unsigned int systemAddressToClientID(RakNet::SystemAddress& systemAddress);

	//Connection functions
	void addNewConnection(RakNet::SystemAddress systemAddress);
	void removeConnection(RakNet::SystemAddress systemAddress);

	void sendClientIDToClient(unsigned int uiClientID);
	GameObject createNewObject(RakNet::BitStream& bsIn, RakNet::SystemAddress& ownerSysAddress);
	void sendGameObjectToAllClients(GameObject& gameObject, RakNet::SystemAddress ownerSystemAddress);

private:
	struct ConnectionInfo
	{
		unsigned int uiConnectionID;
		RakNet::SystemAddress sysAddress;
	};

	const unsigned short PORT = 5456;
	RakNet::RakPeerInterface* m_pPeerInterface;


	unsigned int m_uiConnectionCounter;
	std::unordered_map<unsigned int, ConnectionInfo> m_connectedClients;
	std::vector<GameObject> m_gameObjects;
	unsigned int m_uiObjectCounter;
};
