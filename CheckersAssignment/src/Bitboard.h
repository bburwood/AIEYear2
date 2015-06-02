#ifndef _BITBOARD_H_
#define _BITBOARD_H_

//	This header contains bitboard related definitions
//	The bitboard is implemented as a 32 bit unsigned int, where each bit flags if there is a piece in that square.
//	Only 32 bits are needed as only every second square on the checkerboard is use for the game.
//	This allows us to use just 4 unsigned ints to specify the state of the game:
//		P1 pieces, P1 Kings, P2 pieces, P2 Kings.
//	The Board is centered on the origin, on the X/Z plane, with each square being unit size, with Player 1 at the top
//	of the board in the negative Z direction.
//	In terms of Board coordinates (0,0) is the top left of the board, when viewed from player 2's perspective. (the default view
//	when the game window appears)

#include "GLMHeader.h"

typedef	unsigned int	Bitboard;

//	What do I need?  ...
//	Player 1/2 starting config
//	Array for piece coordinates
//	Bitmasks for "everything":
//		Player 1/2 King row
//		Starting board config

struct GameState
{
	int	m_iCurrentPlayer;	//	who's turn is it?
	//	and where are all the pieces?
	Bitboard	m_P1Pieces;
	Bitboard	m_P1Kings;
	Bitboard	m_P2Pieces;
	Bitboard	m_P2Kings;
};

struct Move
{
	Bitboard	StartPos;
	Bitboard	EndPos;
};

extern	vec3 aPieceCoords[32];
extern	vec3 aKingCoords[32];
extern	Bitboard	bbP1StartPieces;
extern	Bitboard	bbP1StartKings;
extern	Bitboard	bbP2StartPieces;
extern	Bitboard	bbP2StartKings;


extern	void	InitPieceCoordsArray();
extern	Bitboard	GenerateBitMaskFromCoords(int iX, int iZ);
extern	Bitboard	GetCurrentAvailableMoves(int a_iPlayer, Bitboard a_bbP1Pieces, Bitboard a_bbP1Kings, Bitboard a_bbP2Pieces, Bitboard a_bbP2Kings);
extern	Bitboard	CheckForCaptureMoves(int a_iPlayer, Bitboard a_bbP1Pieces, Bitboard a_bbP1Kings, Bitboard a_bbP2Pieces, Bitboard a_bbP2Kings);
extern	Bitboard	GetEmptySquares(Bitboard a_bbP1Pieces, Bitboard a_bbP2Pieces);
//	May need to add a CheckForCaptureContinue function for the multi capture move cases.
extern	int			CountPieces(Bitboard a_bbBoard);


#endif	//	_BITBOARD_H_
