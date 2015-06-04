#include "Game.h"
//#include "gl_core_4_4.h"
//#include <GLFW\glfw3.h>

#include "Bitboard.h"

Game::Game()
{
	InitPieceCoordsArray();
	//	Initiate the default player types.
	m_P1.m_ePlayerType = PLAYER_HUMAN;	//	change this to PLAYER_MCTS_AI when some AI code starts getting written
	m_P2.m_ePlayerType = PLAYER_HUMAN;
}

Game::~Game()
{
}

void	Game::update(float dT)
{
	if (!m_bGameOver)
	{
		m_P1.update(dT, m_oGameState);
		m_P2.update(dT, m_oGameState);
	}
}

void	Game::FindValidMoves()
{
}

void	Game::MakeMove(Move a_oMove)
{
	//	it is required that ValidMove() has been called before this function as a check that the move about to be tried is a valid move.
	if (m_oGameState.m_iCurrentPlayer == 1)
	{
		//	Player one has made a move
		//	first check if a King has moved
		if ((m_oGameState.m_P1Kings & a_oMove.StartPos) > 0)
		{
			//	A King has been moved
			m_oGameState.m_P1Kings = (m_oGameState.m_P1Kings & ~a_oMove.StartPos) | a_oMove.EndPos;
		}
		m_oGameState.m_P1Pieces = (m_oGameState.m_P1Pieces & ~a_oMove.StartPos) | a_oMove.EndPos;
		//	this would also be where a check for a capture move is made and then fire off a particle system for the captured piece
		//	the captured piece would then also need to be removed from the game board
	}
	else
	{
		//	Player two has made a move
		//	first check if a King has moved
		if ((m_oGameState.m_P2Kings & a_oMove.StartPos) > 0)
		{
			//	A King has been moved
			m_oGameState.m_P2Kings = (m_oGameState.m_P2Kings & ~a_oMove.StartPos) | a_oMove.EndPos;
		}
		m_oGameState.m_P2Pieces = (m_oGameState.m_P2Pieces & ~a_oMove.StartPos) | a_oMove.EndPos;
		//	this would also be where a check for a capture move is made and then fire off a particle system for the captured piece
	}
}

void	Game::DecideMove()
{
}

void	Game::EndTurn()
{
	if (m_oGameState.m_iCurrentPlayer == 1)
	{
		m_oGameState.m_iCurrentPlayer = 2;
	}
	else
	{
		m_oGameState.m_iCurrentPlayer = 1;
	}
}

void	Game::ResetGame(int a_iFirstMover)
{
	m_oGameState.m_iCurrentPlayer = a_iFirstMover;
	m_oGameState.m_P1Pieces = bbP1StartPieces;
	m_oGameState.m_P1Kings = bbP1StartKings;
	m_oGameState.m_P2Pieces = bbP2StartPieces;
	m_oGameState.m_P2Kings = bbP2StartKings;
	m_P1.InitPlayer(1, PLAYER_HUMAN, 1.0f, this);	//	change this to PLAYER_MCTS_AI when some AI code starts getting written
	m_P2.InitPlayer(2, PLAYER_HUMAN, 1.0f, this);
	m_bPieceMoving = false;
	m_bMoveStarted = false;
	m_bMoveEnded = false;
	m_bGameOver = false;
}

void	Game::MouseClickedOnBoardAt(int iBoardX, int iBoardY)
{
	if (m_P1.m_ePlayerType == PLAYER_HUMAN)
	{
		m_P1.MouseClickedOnBoardAt(iBoardX, iBoardY);
	}
	if (m_P2.m_ePlayerType == PLAYER_HUMAN)
	{
		m_P2.MouseClickedOnBoardAt(iBoardX, iBoardY);
	}
}

