#include "BasicNetworking.h"

#include <iostream>
#include <string>

#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"


BasicNetworkingApplication::BasicNetworkingApplication()
{

}

BasicNetworkingApplication::~BasicNetworkingApplication()
{

}

bool BasicNetworkingApplication::startup()
{
	//Setup the basic window
	//createWindow("Client Application", 1280, 720);

	handleNetworkConnection();
	return true;
}

void BasicNetworkingApplication::shutdown()
{

}

bool BasicNetworkingApplication::update(float deltaTime)
{
	handleNetworkMessages();
	return true;
}

void BasicNetworkingApplication::draw()
{

}

//Initialize the connection
void BasicNetworkingApplication::handleNetworkConnection()
{
	//Initialize the Raknet peer interface first
	m_pPeerInterface = RakNet::RakPeerInterface::GetInstance();
	initialiseClientConnection();}

void BasicNetworkingApplication::initialiseClientConnection()
{
	//Create a socket descriptor to describe this connection
	//No data needed, as we will be connecting to a server
	RakNet::SocketDescriptor sd;
	//Now call startup - max of 1 connections (to the server)
	m_pPeerInterface->Startup(1, &sd, 1);
	std::cout << "Connectiong to server at: " << IP << std::endl;
	//Now call connect to attempt to connect to the given server
	RakNet::ConnectionAttemptResult res = m_pPeerInterface->Connect(IP, PORT, nullptr, 0);
	//Finally, check to see if we connected, and if not, throw a error
	if (res != RakNet::CONNECTION_ATTEMPT_STARTED)
	{
		std::cout << "Unable to start connection, Error number: " << res << std::endl;
	}
}

//Handle incoming packets
void BasicNetworkingApplication::handleNetworkMessages()
{
	RakNet::Packet* packet;
	for (packet = m_pPeerInterface->Receive(); packet;
		m_pPeerInterface->DeallocatePacket(packet),
		packet = m_pPeerInterface->Receive())
	{
		switch (packet->data[0])
		{
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			std::cout << "Another client has disconnected.\n";
			break;
		case ID_REMOTE_CONNECTION_LOST:
			std::cout << "Another client has lost the connection.\n";
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION:
			std::cout << "Another client has connected.\n";
			break;
		case ID_CONNECTION_REQUEST_ACCEPTED:
			std::cout << "Our connection request has been accepted.\n";
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			std::cout << "The server is full.\n";
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			std::cout << "We have been disconnected.\n";
			break;
		case ID_CONNECTION_LOST:
			std::cout << "Connection lost.\n";
			break;
		case ID_SERVER_TEXT_MESSAGE:
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			RakNet::RakString str;
			bsIn.Read(str);
			std::cout << str.C_String() << std::endl;
			break;
		}
		default:
			std::cout << "Received a message with a unknown id: " << packet->data[0];
			break;
		}
	}
}



