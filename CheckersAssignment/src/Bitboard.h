#ifndef _BITBOARD_H_
#define _BITBOARD_H_

//	This header contains bitboard related definitions
//	The bitboard is implemented as a 32 bit unsigned int, where each bit flags if there is a piece in that square.
//	Only 32 bits are needed as only every second square on the checkerboard is use for the game.
//	This allows us to use just 4 ints to specify the state of the game:
//		P1 pieces, P1 Kings, P2 pieces, P2 Kings.
//	The Board is centered on the origin, on the X/Z plane, with each square being unit size, with Player 1 at the top
//	of the board in the negative Z direction.

#include "GLMHeader.h"

typedef	unsigned int	Bitboard;

//	What do I need?  ...
//	Player 1/2 starting config
//	Array for piece coordinates
//	Bitmasks for "everything":
//		Player 1/2 King row
//		Starting board config

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


#endif	//	_BITBOARD_H_
