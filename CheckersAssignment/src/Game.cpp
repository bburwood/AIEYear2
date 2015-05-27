#include "Game.h"
//#include "gl_core_4_4.h"
//#include <GLFW\glfw3.h>

#include "Bitboard.h"

Game::Game()
{
	InitPieceCoordsArray();
}

Game::~Game()
{
}

void	Game::update(float dT)
{

}

void	Game::FindValidMoves()
{
}

void	Game::MakeMove()
{
}

void	Game::DecideMove()
{
}

void	Game::ResetGame(int a_iFirstMover)
{
	m_P1Pieces = bbP1StartPieces;
	m_P1Kings = bbP1StartKings;
	m_P2Pieces = bbP2StartPieces;
	m_P2Kings = bbP2StartKings;
	m_iCurrentPlayer = a_iFirstMover;
}



