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
#include <vector>

typedef	unsigned int	Bitboard;

//	not really the place to put these, but it serves the purpose - just need to then add the include for this header into Application.cpp as well.
const int	BUFFER_WIDTH = 1920;
const int	BUFFER_HEIGHT = 1080;

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
	Bitboard	m_P2Pieces;
	Bitboard	m_Kings;
};

struct Move
{
	Bitboard	StartPos;
	Bitboard	EndPos;
	bool	bJumper;
};

extern	Bitboard	abbSquareMasks[32];
extern	vec3 aPieceCoords[32];
extern	vec3 aKingCoords[32];
extern	Bitboard	bbP1StartPieces;
extern	Bitboard	bbP2StartPieces;
extern	Bitboard	bbStartKings;
extern	Bitboard	bbP1KingRow;
extern	Bitboard	bbP2KingRow;
extern	Bitboard	bbRow0;
extern	Bitboard	bbRow1;
extern	Bitboard	bbRow2;
extern	Bitboard	bbRow3;
extern	Bitboard	bbRow4;
extern	Bitboard	bbRow5;
extern	Bitboard	bbRow6;
extern	Bitboard	bbRow7;
extern	Bitboard	bbCentreSquares;
extern	Bitboard	bbAllEdgePlaces;

extern	void	InitPieceCoordsArray();
extern	Bitboard	GenerateBitMaskFromCoords(int iX, int iZ);
extern	int	GetBitIndexFromXY(int iX, int iZ);
extern	int	GetBitIndex(Bitboard a_bbBoard);
extern	int	GetBoardXCoord(Bitboard a_bbBoard);
extern	int	GetBoardYCoord(Bitboard a_bbBoard);
extern	int	GetXOffsetFromRow(int a_iRow);
extern	int	GetBoardXCoordFromIndex(int a_iIndex);
extern	int	GetBoardYCoordFromIndex(int a_iIndex);
extern	int	GetFirstBitNumber(Bitboard a_bbBoard);
extern	Bitboard	GetPlayerJumpers(int a_iPlayer, GameState a_oCurrentState);
extern	Bitboard	GetCurrentAvailableMovers(GameState a_oCurrentState, bool &a_bJumpers);
extern	Bitboard	CheckForCaptureMoves(GameState a_oCurrentState);
extern	Bitboard	GetEmptySquares(Bitboard a_bbP1Pieces, Bitboard a_bbP2Pieces);
//	May need to add a CheckForCaptureContinue function for the multi capture move cases.
extern	int		CountPieces(Bitboard a_bbBoard);
extern	bool	ValidMove(Move a_oTestMove, GameState a_oCurrentState);
extern	void	GenerateMovesForMover(int a_iMover, GameState a_oGameState, std::vector<Move> &a_aMoveList, bool a_bJumper);
extern	bool	IsCaptureMove(Move a_oMove);

#endif	//	_BITBOARD_H_
