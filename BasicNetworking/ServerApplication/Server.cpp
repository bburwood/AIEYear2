#include "Server.h"
#include "GameObject.h"

Server::Server()
{
	//Initialize the Raknet peer interface first
	m_pPeerInterface = RakNet::RakPeerInterface::GetInstance();
	//	Simulate latency and packet loss - comment out for release builds
	m_pPeerInterface->ApplyNetworkSimulator(0.0f, 150, 50);

	m_uiConnectionCounter = 1;
	m_uiObjectCounter = 1;
}

Server::~Server()
{

}

void Server::run()
{

	//Startup the server, and start it listening to clients
	std::cout << "Starting up the server..." << std::endl;

	//Create a socket descriptor to describe this connection
	RakNet::SocketDescriptor sd(PORT, 0);

	//Now call startup - max of 32 connections, on the assigned port
	m_pPeerInterface->Startup(32, &sd, 1);
	m_pPeerInterface->SetMaximumIncomingConnections(32);

	handleNetworkMessages();
}


void Server::handleNetworkMessages()
{
	RakNet::Packet* packet = nullptr;

	while (1)
	{
		for (packet = m_pPeerInterface->Receive(); packet; m_pPeerInterface->DeallocatePacket(packet), packet = m_pPeerInterface->Receive())
		{
			switch (packet->data[0])
			{
			case ID_NEW_INCOMING_CONNECTION:
			{
				addNewConnection(packet->systemAddress);
				std::cout << "A connection is incoming.\n";
				sendAllGameObjectsToClient(packet->systemAddress);
				break;
			}
			case ID_CLIENT_CREATE_OBJECT:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				GameObject newObject = createNewObject(bsIn, packet->systemAddress);
				sendGameObjectToAllClients(newObject, RakNet::UNASSIGNED_SYSTEM_ADDRESS);
				break;
			}
			case ID_CLIENT_UPDATE_OBJECT_POSITION:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				updateObject(bsIn, packet->systemAddress);
				//	update object will also send the updated object to all clients if this is a legal update
				break;
			}
			case ID_DISCONNECTION_NOTIFICATION:
				std::cout << "A client has disconnected.\n";
				removeConnection(packet->systemAddress);
				break;
			case ID_CONNECTION_LOST:
				std::cout << "A client lost the connection.\n";
				removeConnection(packet->systemAddress);
				break;
			default:
				std::cout << "Received a message with a unknown id: " << packet->data[0];
				break;
			}
		}
	}
}


void Server::addNewConnection(RakNet::SystemAddress systemAddress)
{
	ConnectionInfo info;
	info.sysAddress = systemAddress;
	info.uiConnectionID = m_uiConnectionCounter++;
	m_connectedClients[info.uiConnectionID] = info;

	sendClientIDToClient(info.uiConnectionID);
}

void Server::removeConnection(RakNet::SystemAddress systemAddress)
{
	for (auto it = m_connectedClients.begin(); it != m_connectedClients.end(); it++)
	{
		if (it->second.sysAddress == systemAddress)
		{
			m_connectedClients.erase(it);
			break;
		}
	}
}

unsigned int Server::systemAddressToClientID(RakNet::SystemAddress& systemAddress)
{
	for (auto it = m_connectedClients.begin(); it != m_connectedClients.end(); it++)
	{
		if (it->second.sysAddress == systemAddress)
		{
			return it->first;
		}
	}

	return 0;
}


void Server::sendClientIDToClient(unsigned int uiClientID)
{
	RakNet::BitStream bs;
	bs.Write((RakNet::MessageID)GameMessages::ID_SERVER_CLIENT_ID);
	bs.Write(uiClientID);

	m_pPeerInterface->Send(&bs, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, m_connectedClients[uiClientID].sysAddress, false);
}

GameObject Server::createNewObject(RakNet::BitStream& bsIn, RakNet::SystemAddress& ownerSysAddress)
{
	GameObject newGameObject;
	//Read in the information from the packet
	bsIn.Read(newGameObject.fXPos);
	bsIn.Read(newGameObject.fZPos);
	bsIn.Read(newGameObject.fXVel);
	bsIn.Read(newGameObject.fZVel);
	bsIn.Read(newGameObject.fRedColour);
	bsIn.Read(newGameObject.fGreenColour);
	bsIn.Read(newGameObject.fBlueColour);

	newGameObject.uiOwnerClientID = systemAddressToClientID(ownerSysAddress);
	newGameObject.uiObjectID = m_uiObjectCounter++;

	m_gameObjects.push_back(newGameObject);	//	add object to the server;s object list

	return newGameObject;
}

void Server::sendGameObjectToAllClients(GameObject& gameObject, RakNet::SystemAddress ownerSystemAddress)
{
	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)GameMessages::ID_SERVER_FULL_OBJECT_DATA);
	bsOut.Write(gameObject.fXPos);
	bsOut.Write(gameObject.fZPos);
	bsOut.Write(gameObject.fXVel);
	bsOut.Write(gameObject.fZVel);
	bsOut.Write(gameObject.fRedColour);
	bsOut.Write(gameObject.fGreenColour);
	bsOut.Write(gameObject.fBlueColour);
	bsOut.Write(gameObject.uiOwnerClientID);
	bsOut.Write(gameObject.uiObjectID);
	m_pPeerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, ownerSystemAddress, true);
}

void Server::sendGameObjectToClient(GameObject& gameObject, RakNet::SystemAddress ownerSystemAddress)
{
	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)GameMessages::ID_SERVER_FULL_OBJECT_DATA);
	bsOut.Write(gameObject.fXPos);
	bsOut.Write(gameObject.fZPos);
	bsOut.Write(gameObject.fXVel);
	bsOut.Write(gameObject.fZVel);
	bsOut.Write(gameObject.fRedColour);
	bsOut.Write(gameObject.fGreenColour);
	bsOut.Write(gameObject.fBlueColour);
	bsOut.Write(gameObject.uiOwnerClientID);
	bsOut.Write(gameObject.uiObjectID);
	m_pPeerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, ownerSystemAddress, false);
}

void Server::sendAllGameObjectsToClient(RakNet::SystemAddress ownerSystemAddress)
{
	for (unsigned int i = 0; i < m_gameObjects.size(); ++i)
	{
		sendGameObjectToClient(m_gameObjects[i], ownerSystemAddress);
	}
}
void Server::updateObject(RakNet::BitStream& bsIn, RakNet::SystemAddress& ownerSysAddress)
{
	unsigned int uiObjectID;
	float fXPos;
	float fZPos;
	float fXVel;
	float fZVel;
	bsIn.Read(uiObjectID);
	bsIn.Read(fXPos);
	bsIn.Read(fZPos);
	bsIn.Read(fXVel);
	bsIn.Read(fZVel);

	unsigned int uiSenderID = systemAddressToClientID(ownerSysAddress);

	for (int i = 0; i < m_gameObjects.size(); i++)
	{
		if (m_gameObjects[i].uiObjectID == uiObjectID)
		{
			//	we have found the object, now check if the sender actually owns it!!
			if (m_gameObjects[i].uiOwnerClientID == uiSenderID)
			{
				//	yes they own it - so they're not trying to cheat!  So update the object's position.
				m_gameObjects[i].fXPos = fXPos;
				m_gameObjects[i].fZPos = fZPos;
				m_gameObjects[i].fXPos = fXVel;
				m_gameObjects[i].fZPos = fZVel;
				//	now send the updated object to all clients, except the owner that updated the object
				sendGameObjectToAllClients(m_gameObjects[i], ownerSysAddress);
				//	ok, now the object has now been updated, get out of the loop
				break;
			}
		}
	}
}
