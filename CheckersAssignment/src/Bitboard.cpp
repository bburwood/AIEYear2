#include "Bitboard.h"
#include <math.h>

//	Bitmask for each position on the board
Bitboard	abbSquareMasks[32] = { 
	0x00000001,
	0x00000002,
	0x00000004,
	0x00000008,
	0x00000010,
	0x00000020,
	0x00000040,
	0x00000080,
	0x00000100,
	0x00000200,
	0x00000400,
	0x00000800,
	0x00001000,
	0x00002000,
	0x00004000,
	0x00008000,
	0x00010000,
	0x00020000,
	0x00040000,
	0x00080000,
	0x00100000,
	0x00200000,
	0x00400000,
	0x00800000,
	0x01000000,
	0x02000000,
	0x04000000,
	0x08000000,
	0x10000000,
	0x20000000,
	0x40000000,
	0x80000000,
};
//	the coordinates of the game squares - need to have 2 arrays as the Kings are at a higher Y coordinate
vec3 aPieceCoords[32];
vec3 aKingCoords[32];
//	*****---->>>> remember to use the *bitwise* not operator '~' NOT the *logical/boolean* not operator '!'
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

int	GetFirstBitNumber(Bitboard a_bbBoard)
{
	//	takes in a bit board and returns the index of the first bit set
	//	it will return -1 of no bits are set - which shouldn't happen for this game
	int	iResult = -1;
	for (int i = 0; i < 32; ++i)
	{
		if ((a_bbBoard & (1 << i)) > 0)
		{
			//	this is the bit we are looking for
			iResult = i;
			break;
		}
	}
	return iResult;
}

Bitboard	GetCurrentAvailableMovers(GameState a_oCurrentState)
{
	//	This function checks for current moves available to the player passed in.  It first check for any capture moves, and if none are
	//	found then looks for non-capture moves.
	//	It returns a bitboard containing all pieces that can make a valid move.
	Bitboard	bbResult = 0;	//	start with an empty bitboard
	bbResult = CheckForCaptureMoves(a_oCurrentState);
	if (bbResult > 0)
	{
		//	we have capture moves available, return the pieces able to capture
		return bbResult;
	}
	//	No capture moves available, so check for non-capture moves
	Bitboard	bbAllPieces = a_oCurrentState.m_P1Pieces | a_oCurrentState.m_P2Pieces;
	Bitboard	bbEmptySquares = ~bbAllPieces;
	if (a_oCurrentState.m_iCurrentPlayer == 1)
	{
		Bitboard	bbP1AllShift4 = (bbEmptySquares >> 4) & a_oCurrentState.m_P1Pieces;	//	can a piece move to the main next row position
		bbResult |= bbP1AllShift4;	//	add this to the resulting moves
		//	and then we need bitmasks ...

	}
	else
	{
		//	a_iPlayer == 2
		Bitboard	bbP2AllShift4 = (a_oCurrentState.m_P1Pieces >> 4) & (~bbAllPieces);	//	can a piece move to the main next row position
	}

	//	Just for testing!!!!
//	return 0x00000F00;

	return bbResult;
}

Bitboard	CheckForCaptureMoves(GameState a_oCurrentState)
{
	//	This function checks for any capture moves available for the player passed in, and returns a BitBoard containing all pieces able to
	//	capture in the current move.  If no capture moves are available the returned Bitboard will be empty (0).
	Bitboard	bbResult = 0;	//	start with an empty bitboard
	if (a_oCurrentState.m_iCurrentPlayer == 1)
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

bool	ValidMove(Move a_oTestMove, GameState a_oCurrentState)
{
	//	This function takes in a move and a game state and returns whether the move is a valid move or not
	//	Useful information found on this web page:
	//	http://www.3dkingdoms.com/checkers/bitboards.htm
	Bitboard	bbStartPos = a_oTestMove.StartPos;
	Bitboard	bbEndPos = a_oTestMove.EndPos;
	Bitboard	bbEmptySquares = GetEmptySquares(a_oCurrentState.m_P1Pieces, a_oCurrentState.m_P2Pieces);
	if (a_oCurrentState.m_iCurrentPlayer == 1)
	{
		//	check if a valid move for player 1
		//	first check if the start position is actually player 1's piece!
		if ((bbStartPos & a_oCurrentState.m_P1Pieces) == 0)
		{
			//	someone's trying to move the other player's piece!
			return false;
		}
		//	next check if this is a king
		if ((bbStartPos & a_oCurrentState.m_P1Kings) > 0)
		{
			//	it is a king so check for valid king moves
			return false;
		}
		else
		{
			//	it is not a king, so check for valid man moves
			//	first check for the standard shift 4 "zigzag" down the board
//			Bitboard	bbP1AllShift4 = (a_bbP1Pieces << 4) & (~bbAllPieces);	//	can a piece move to the main next row position
			if (((bbStartPos << 4) & bbEmptySquares & bbEndPos) > 0)
			{
				return true;
			}
		}
	}
	else
	{
		//	check if a valid move for player 2
	}

	return false;	//	just for testing ...

	//	return bResult;
}

Bitboard	GetEmptySquares(Bitboard a_bbP1Pieces, Bitboard a_bbP2Pieces)
{
	//	this one should probably be a macro
	return (~(a_bbP1Pieces | a_bbP2Pieces));	//	remember to use the *bitwise* not operator '~' NOT the *logical/boolean* not operator '!'
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

bool	IsCaptureMove(Move a_oMove)
{
	//	basically need to get what bit the start and end positions are in and if the difference is greater than 6 then it was a capture move
	int	iStartIndex = GetFirstBitNumber(a_oMove.StartPos);
	int	iEndIndex = GetFirstBitNumber(a_oMove.EndPos);

	if (abs(iStartIndex - iEndIndex) > 6)
		return true;
	else
		return false;
}
