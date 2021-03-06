#pragma once

#include "MessageIdentifiers.h"

enum GameMessages
{
	ID_SERVER_TEXT_MESSAGE = ID_USER_PACKET_ENUM + 1,
	ID_SERVER_CLIENT_ID = ID_USER_PACKET_ENUM + 2,
	ID_CLIENT_CREATE_OBJECT = ID_USER_PACKET_ENUM + 3,
	ID_SERVER_FULL_OBJECT_DATA = ID_USER_PACKET_ENUM + 4,
	ID_CLIENT_UPDATE_OBJECT_POSITION = ID_USER_PACKET_ENUM + 5,
	ID_CLIENT_NEW_INPUT = ID_USER_PACKET_ENUM + 6,
};
/*
enum KeyState
{
	RELEASED = 0,
	PRESSED,
};

enum KeyPress
{
	KEY_UP = ID_USER_PACKET_ENUM + 10,
	KEY_DOWN = ID_USER_PACKET_ENUM + 11,
	KEY_LEFT = ID_USER_PACKET_ENUM + 12,
	KEY_RIGHT = ID_USER_PACKET_ENUM + 13,
};
*/