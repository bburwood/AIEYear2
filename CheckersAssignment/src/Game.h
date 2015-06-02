#ifndef _GAME_H_
#define _GAME_H_

#include "Bitboard.h"
#include "Player.h"


class Game
{
public:
	Game();
	~Game();
	void	update(float dT);
	void	FindValidMoves();
	void	MakeMove(Move a_oMove);
	void	DecideMove();
	void	ResetGame(int a_iFirstMover);
	void	MouseClickedOnBoardAt(int iBoardX, int iBoardY);
//	void	xxx();

	GameState	m_oGameState;	//	current state of the game

private:
	Player	m_P1;
	Player	m_P2;
	Move	m_MoveList;
	Move	m_SelectedMove;
	bool	m_bPieceMoving;	//	flags when a move is in progress, for animation purposes.
	bool	m_bMoveStarted;	//	set to true once a player has selected the first move for this turn.
	//	This is so that the game will still force any further captures for the current turn.
	bool	m_bMoveEnded;	//	When m_bMoveStarted is true, this is set to true once the current player can no longer capture any further pieces, ending their turn.
	bool	m_bGameOver;
};

#endif	//	_GAME_H_
