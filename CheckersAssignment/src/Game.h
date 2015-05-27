#ifndef _GAME_H_
#define _GAME_H_

#include "Bitboard.h"

class Game
{
public:
	Game();
	~Game();
	void	update(float dT);
	void	FindValidMoves();
	void	MakeMove();
	void	DecideMove();
	void	ResetGame(int a_iFirstMover);
//	void	xxx();

	int		m_iCurrentPlayer;
	Bitboard	m_P1Pieces;
	Bitboard	m_P1Kings;
	Bitboard	m_P2Pieces;
	Bitboard	m_P2Kings;

private:
	bool	m_bMoving;	//	flags when a move is in progress, for animation purposes.
	//		P1 pieces, P1 Kings, P2 pieces, P2 Kings.
	Move	m_MoveList;
	Move	m_SelectedMove;
};

#endif	//	_GAME_H_
