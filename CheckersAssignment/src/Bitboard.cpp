#include "Bitboard.h"

//	the coordinates of the game squares - need to have 2 arrays as the Kings are at a higher Y coordinate
vec3 aPieceCoords[32];
vec3 aKingCoords[32];
//	now the bitmasks
Bitboard	bbP1StartPieces = 0x00000FFF;	//	= 4,095  = top 3 rows
Bitboard	bbP1StartKings = 0;	//	start with no kings
Bitboard	bbP2StartPieces = 0xFFF00000;	//	= 4,293,918,720	= bottom 3 rows
Bitboard	bbP2StartKings = 0;	//	start with no kings
Bitboard	bbAllEdgePlaces = 0xF818181F;	//	bitmask for all places on the outside row / column of the board
Bitboard	bbP1KingRow = 0x0000000F;
Bitboard	bbP2KingRow = 0xF0000000;


void	InitPieceCoordsArray()
{
	float	fPieceYCoord = 0.080f;
	float	fKingYCoord = 0.17f;
	float	fStartX = -3.5f;
	float	fStartZ = -3.5f;
	int		iOffsetRow = 1;	//	"top" row of the board is offset
	unsigned int uiIndex = 0;
	float	fXOffset;
	float	fZOffset = fStartZ;
	for (int z = 0; z < 8; ++z)
	{
		fXOffset = fStartX + (float)iOffsetRow;
		for (int x = 0; x < 4; ++x)
		{
			aPieceCoords[uiIndex] = vec3(fXOffset, fPieceYCoord, fZOffset);
			aKingCoords[uiIndex] = vec3(fXOffset, fKingYCoord, fZOffset);
			fXOffset += 2.0f;
			++uiIndex;
		}
		fZOffset += 1.0f;
		iOffsetRow = (iOffsetRow + 1) % 2;	//	makes every second row offset
	}
}

Bitboard	GenerateBitMaskFromCoords(int iX, int iZ)
{
	//	assumes that coordinates passed in are valid checkerboard coords
	Bitboard	bbResult = 0;	//	start with an empty bitboard
	if ((iX == -1) || (iZ == -1))
	{
		//	then one of the coordinates is not on the board
		return bbResult;
	}
	int	iBitIndex = (iZ * 4) + (iX / 2);
	bbResult = 1u << iBitIndex;
	return bbResult;
}

Bitboard	GetCurrentAvailableMoves(int a_iPlayer, Bitboard a_bbP1Pieces, Bitboard a_bbP1Kings, Bitboard a_bbP2Pieces, Bitboard a_bbP2Kings)
{
	//	This function checks for current moves available to the player passed in.  It first check for any capture moves, and if none are
	//	found then looks for non-capture moves.
	//	It returns a bitboard containing all pieces that can make a valid move.
	Bitboard	bbResult = 0;	//	start with an empty bitboard
	bbResult = CheckForCaptureMoves(a_iPlayer, a_bbP1Pieces, a_bbP1Kings, a_bbP2Pieces, a_bbP2Kings);
	if (bbResult > 0)
	{
		//	we have capture moves available, return the pieces able to capture
		return bbResult;
	}
	//	No capture moves available, so check for non-capture moves
	Bitboard	bbAllPieces = a_bbP1Pieces | a_bbP2Pieces;
	if (a_iPlayer == 1)
	{
		Bitboard	bbP1AllShift4 = (a_bbP1Pieces << 4) & (!bbAllPieces);	//	can a piece move to the main next row position
		//	and then we need bitmasks ...

	}
	else
	{
		//	a_iPlayer == 2
		Bitboard	bbP2AllShift4 = (a_bbP1Pieces >> 4) & (!bbAllPieces);	//	can a piece move to the main next row position
	}

	//	Just for testing!!!!
	return 0x000FF000;
	//return bbResult;
}

Bitboard	CheckForCaptureMoves(int a_iPlayer, Bitboard a_bbP1Pieces, Bitboard a_bbP1Kings, Bitboard a_bbP2Pieces, Bitboard a_bbP2Kings)
{
	//	This function checks for any capture moves available for the player passed in, and returns a BitBoard containing all pieces able to
	//	capture in the current move.  If no capture moves are available the returned Bitboard will be empty (0).
	Bitboard	bbResult = 0;	//	start with an empty bitboard
	if (a_iPlayer == 1)
	{
		for (unsigned int i = 0; i < 32; ++i)
		{
			//	check each player 1 piece to see if it can capture anything
		}
	}
	else
	{
		for (unsigned int i = 31; i >= 0; ++i)
		{

		}
	}

	return bbResult;
}

Bitboard	GetEmptySquares(Bitboard a_bbP1Pieces, Bitboard a_bbP2Pieces)
{
	//	this one should probably be a macro
	return (!(a_bbP1Pieces & a_bbP2Pieces));
}

int	CountPieces(Bitboard a_bbBoard)
{
	//	This function simply takes in a bitboard and returns the number of pieces it contains
	//	This could also be done as a macro to potentially improve speed ... I may try that later ...
	int	iResult = 0;

	for (unsigned int i = 0; i < 32; ++i)
	{
		if ((a_bbBoard & (0x00000001 << i)) > 0)
		{
			++iResult;
		}
	}

	return iResult;
}

