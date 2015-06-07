#include "Game.h"
#include "Checkers.h"	//	again ... ONLY used for telling the program to fire off the particle emitters for a capture move when made ...

//#include "gl_core_4_4.h"
//#include <GLFW\glfw3.h>

#include "Bitboard.h"

Game::Game()
{
	InitPieceCoordsArray();
	//	Initiate the default player types.
	m_P1.m_ePlayerType = PLAYER_HUMAN;	//	change this to PLAYER_MCTS_AI when some AI code starts getting written
	m_P2.m_ePlayerType = PLAYER_HUMAN;
	m_aMoveList.reserve(32);	//	probably not likely to be more than 32 available moves in any given turn ... but you never know, which is why this is a std::vector
	m_iTurnsSinceLastCapture = 0;
	m_iNoCaptureTurnsLimit = 20;	//	should be 10 turns each (at least for testing - push this to at least 40 for the final version)

}

Game::~Game()
{
}

void	Game::update(float dT)
{
	if (!m_bGameOver)
	{
		if (m_oGameState.m_iCurrentPlayer == 1)
		{
			m_P1.update(dT, m_oGameState);
			if (m_oGameState.m_iCurrentPlayer == 2)
			{
				//	this should force the player to update its internal state to match
				m_P1.update(dT, m_oGameState);
			}
		}
		else
		{
			m_P2.update(dT, m_oGameState);
			if (m_oGameState.m_iCurrentPlayer == 1)
			{
				//	this should force the player to update its internal state to match
				m_P2.update(dT, m_oGameState);
			}
		}
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
		if ((m_oGameState.m_Kings & a_oMove.StartPos) > 0)
		{
			//	A King has been moved
			m_oGameState.m_Kings = (m_oGameState.m_Kings & ~a_oMove.StartPos) | a_oMove.EndPos;
		}
		m_oGameState.m_P1Pieces = (m_oGameState.m_P1Pieces & ~a_oMove.StartPos) | a_oMove.EndPos;
		//	this would also be where a check for a capture move is made and then fire off a particle system for the captured piece
		//	the captured piece would then also need to be removed from the game board
	}
	else
	{
		//	Player two has made a move
		//	first check if a King has moved
		if ((m_oGameState.m_Kings & a_oMove.StartPos) > 0)
		{
			//	A King has been moved
			m_oGameState.m_Kings = (m_oGameState.m_Kings & ~a_oMove.StartPos) | a_oMove.EndPos;
		}
		m_oGameState.m_P2Pieces = (m_oGameState.m_P2Pieces & ~a_oMove.StartPos) | a_oMove.EndPos;
		//	this would also be where a check for a capture move is made and then fire off a particle system for the captured piece
	}
}

void	Game::RemoveCapturedPiece(Bitboard a_bbCaptured)
{
	//	takes in a bitboard for a captured piece and removes that piece from the game board.
	Bitboard	bbNotCaptured = ~a_bbCaptured;
	m_oGameState.m_Kings &= bbNotCaptured;
	m_oGameState.m_P1Pieces &= bbNotCaptured;
	m_oGameState.m_P2Pieces &= bbNotCaptured;
	m_iTurnsSinceLastCapture = 0;
}

void	Game::GetSelectedMoveDetails(bool& a_bPieceSelected, int& a_iXCoord, int& a_iZCoord)
{
	if (m_oGameState.m_iCurrentPlayer == 1)
	{
		m_P1.GetSelectedMoveDetails(a_bPieceSelected, a_iXCoord, a_iZCoord);
	}
	else
	{
		m_P2.GetSelectedMoveDetails(a_bPieceSelected, a_iXCoord, a_iZCoord);
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
	++m_iTurnsSinceLastCapture;
	//	now we need to check for end of game conditions
	CheckForGameOver();
}

void	Game::CheckForGameOver()
{
	int	iP1Pieces = CountPieces(m_oGameState.m_P1Pieces);
	int	iP2Pieces = CountPieces(m_oGameState.m_P2Pieces);
	if (iP1Pieces == 0)
	{
		//	Player 2 wins
		m_bGameOver = true;
		//	fire off Player 2 winning particle effects
		for (int i = 0; i < 64; ++i)
		{
			m_pProgram->FireGameOverEmitterAt(i % 8, 7, 0.5f, 2);
		}
	}
	if (iP2Pieces == 0)
	{
		//	Player 1 wins
		m_bGameOver = true;
		//	fire off Player 1 winning particle effects
		for (int i = 0; i < 64; ++i)
		{
			m_pProgram->FireGameOverEmitterAt(i % 8, 1, 0.5f, 1);
		}
	}
	//	otherwise do a check for the number of turns since the last piece was taken, and end the game if it is too long
	if (m_iTurnsSinceLastCapture > m_iNoCaptureTurnsLimit)
	{
		//	then it's game over - sorry you didn't capture a piece for too long so nobody wins!!
		m_bGameOver = true;
		for (int i = 0; i < 64; ++i)
		{
			m_pProgram->FireGameOverEmitterAt(i % 8, i / 8, 0.5f, 0);
		}
	}
}

void	Game::ResetGame(int a_iFirstMover)
{
	m_oGameState.m_iCurrentPlayer = a_iFirstMover;
	m_oGameState.m_P1Pieces = bbP1StartPieces;
	m_oGameState.m_P2Pieces = bbP2StartPieces;
	m_oGameState.m_Kings = bbStartKings;
	m_iTurnsSinceLastCapture = 0;
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

void	Game::SetCheckersPointer(Checkers* a_pCheckers)
{
	m_pProgram = a_pCheckers;
}

void	Game::SetPlayerCheckersPointer(Checkers* a_pCheckers)
{
	m_P1.SetCheckersPointer(a_pCheckers);
	m_P2.SetCheckersPointer(a_pCheckers);
}
