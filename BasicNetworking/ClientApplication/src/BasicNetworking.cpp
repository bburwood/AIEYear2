#include "BasicNetworking.h"

#include <iostream>
#include <string>

#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"

#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include "Gizmos.h"
#include <stdlib.h>     /* srand, rand */

#include "../../ServerApplication/GameObject.h"

BasicNetworkingApplication::BasicNetworkingApplication()
{
	//	select a random colour
	m_myColour = glm::vec4( (float)rand() / (float)RAND_MAX,
							(float)rand() / (float)RAND_MAX,
							(float)rand() / (float)RAND_MAX, 1.0f);
	std::cout << "My Colour: " << m_myColour.r << ", " << m_myColour.g << ", " << m_myColour.b << ", " << '\n';
}

BasicNetworkingApplication::~BasicNetworkingApplication()
{

}

bool BasicNetworkingApplication::startup()
{
	//Setup the basic window
	createWindow("Client Application", 1280, 720);
	m_camera = Camera(0, 0, 0, 0);
	Gizmos::create();


	handleNetworkConnection();
	return true;
}

void BasicNetworkingApplication::shutdown()
{

}

bool BasicNetworkingApplication::update(float deltaTime)
{
	handleNetworkMessages();
	moveClientObject(deltaTime);
	m_camera.update(deltaTime);
	return true;
}

void BasicNetworkingApplication::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Gizmos::clear();

	for (int i = 0; i <= 10; ++i)
	{
		Gizmos::addLine(glm::vec3(-5.0f, 0.0f, -5 + i), glm::vec3(5.0f, 0.0f, -5 + i), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		Gizmos::addLine(glm::vec3(-5.0f + i, 0.0f, -5), glm::vec3(-5.0f + i, 0.0f, 5), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	}

	for (unsigned int i = 0; i < m_gameObjects.size(); ++i)
	{
		GameObject* obj = &m_gameObjects[i];
		Gizmos::addSphere(glm::vec3(obj->fXPos, 1.0f, obj->fZPos), 1.0f, 16, 16, glm::vec4(obj->fRedColour, obj->fGreenColour, obj->fBlueColour, 1.0f));
		//Gizmos::addSphere(glm::vec3(0, 2, 0), 1.0f, 10, 10, m_myColour);
	}

	Gizmos::draw(m_camera.getProjectionView());

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
	std::cout << "Connecting to server at: " << IP << std::endl;
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
		//	GameMessage ID's below:
		case ID_SERVER_TEXT_MESSAGE:
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			RakNet::RakString str;
			bsIn.Read(str);
			std::cout << str.C_String() << std::endl;
			break;
		}
		case ID_SERVER_CLIENT_ID:
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			bsIn.Read(m_uiClientId);
			std::cout << "Server has given us an id of: " << m_uiClientId << std::endl;
			createGameObject();			break;
		}
		case ID_SERVER_FULL_OBJECT_DATA:
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			readObjectDataFromServer(bsIn);
			break;
		}
		default:
			std::cout << "Received a message with a unknown id: " << packet->data[0];
			break;
		}
	}	
}

void BasicNetworkingApplication::readObjectDataFromServer(RakNet::BitStream& bsIn)
{
	//Create a temp object that we will pull all the object data into
	GameObject tempGameObject;
	//Read in the object data
	bsIn.Read(tempGameObject.fXPos);
	bsIn.Read(tempGameObject.fZPos);
	bsIn.Read(tempGameObject.fRedColour);
	bsIn.Read(tempGameObject.fGreenColour);
	bsIn.Read(tempGameObject.fBlueColour);
	bsIn.Read(tempGameObject.uiOwnerClientID);
	bsIn.Read(tempGameObject.uiObjectID);
	//Check to see whether or not this object is already stored in our local object list
	bool bFound = false;
	for (int i = 0; i < m_gameObjects.size(); i++)
	{
		if (m_gameObjects[i].uiObjectID == tempGameObject.uiObjectID)
		{
			bFound = true;
			//Update the game object
			GameObject& obj = m_gameObjects[i];
			obj.fXPos = tempGameObject.fXPos;
			obj.fZPos = tempGameObject.fZPos;
			obj.fRedColour = tempGameObject.fRedColour;
			obj.fGreenColour = tempGameObject.fGreenColour;
			obj.fBlueColour = tempGameObject.fBlueColour;
			obj.uiOwnerClientID = tempGameObject.uiOwnerClientID;
			break;
		}
	}
	//If we didn't find it, then it is a new object - add it to our object list
	if (!bFound)
	{
		m_gameObjects.push_back(tempGameObject);
		if (tempGameObject.uiOwnerClientID == m_uiClientId)
		{
			m_uiclientObjectIndex = m_gameObjects.size() - 1;
		}
	}
}

void BasicNetworkingApplication::createGameObject()
{
	//Tell the server we want to create a new game object that will represent us
	RakNet::BitStream bsOut;
	GameObject tempGameObject;
	tempGameObject.fXPos = 0.0f;
	tempGameObject.fZPos = 0.0f;
	tempGameObject.fRedColour = m_myColour.r;
	tempGameObject.fGreenColour = m_myColour.g;
	tempGameObject.fBlueColour = m_myColour.b;
	//Ensure that the write order is the same as the read order on the server!
	bsOut.Write((RakNet::MessageID)GameMessages::ID_CLIENT_CREATE_OBJECT);
	bsOut.Write(tempGameObject.fXPos);
	bsOut.Write(tempGameObject.fZPos);
	bsOut.Write(tempGameObject.fRedColour);
	bsOut.Write(tempGameObject.fGreenColour);
	bsOut.Write(tempGameObject.fBlueColour);
	m_pPeerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
		RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void BasicNetworkingApplication::moveClientObject(float deltaTime)
{
	//We don't have a valid client ID, so we we have no game object!
	if (m_uiClientId == 0) return;
	//No game objects sent to us, so we don't know who we are yet
	if (m_gameObjects.size() == 0) return;
	bool bUpdatedObjectPosition = false;
	GameObject& myClientObject = m_gameObjects[m_uiclientObjectIndex];
	if (glfwGetKey(m_window, GLFW_KEY_UP))
	{
		myClientObject.fZPos += 2 * deltaTime;
		bUpdatedObjectPosition = true;
	}
	if (glfwGetKey(m_window, GLFW_KEY_DOWN))
	{
		myClientObject.fZPos -= 2 * deltaTime;
		bUpdatedObjectPosition = true;
	}
	if (bUpdatedObjectPosition == true)
	{
		sendUpdatedObjectPositionToServer(myClientObject);
	}
}

void BasicNetworkingApplication::sendUpdatedObjectPositionToServer(GameObject& a_myClientObject)
{
	//	tell the server that our object's position has changed
	RakNet::BitStream bsOut;

	bsOut.Write((RakNet::MessageID)GameMessages::ID_CLIENT_UPDATE_OBJECT_POSITION);
	bsOut.Write(a_myClientObject.uiObjectID);
	bsOut.Write(a_myClientObject.fXPos);
	bsOut.Write(a_myClientObject.fZPos);
	m_pPeerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
		RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}
