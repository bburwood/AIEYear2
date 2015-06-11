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
Bitboard	bbP2StartPieces = 0xFFF00000;	//	= 4,293,918,720	= bottom 3 rows
Bitboard	bbStartKings = 0;	//	start with no kings
Bitboard	bbAllEdgePlaces = 0xF818181F;	//	bitmask for all places on the outside row / column of the board
Bitboard	bbP1KingRow = 0xF0000000;
Bitboard	bbP2KingRow = 0x0000000F;
Bitboard	bbRow0 = 0x0000000F;
Bitboard	bbRow1 = 0x000000F0;
Bitboard	bbRow2 = 0x00000F00;
Bitboard	bbRow3 = 0x0000F000;
Bitboard	bbRow4 = 0x000F0000;
Bitboard	bbRow5 = 0x00F00000;
Bitboard	bbRow6 = 0x0F000000;
Bitboard	bbRow7 = 0xF0000000;
Bitboard	bbCentreSquares = 0x00666600;
Bitboard	bbP1L3 = 0x00E0E0E0;	//	squares for which it is legal for a P1 piece to move to the place shifted +3
Bitboard	bbP1L5 = 0x07070707;	//	squares for which it is legal for a P1 piece to move to the place shifted +5
Bitboard	bbP2R3 = 0x07070700;	//	squares for which it is legal for a P2 piece to move to the place shifted -3
Bitboard	bbP2R5 = 0xE0E0E0E0;	//	squares for which it is legal for a P2 piece to move to the place shifted -5

//	bitmasks for jump moves
Bitboard	bbJumpUpRightEnd = 0x00EEEEEE;	//	squares that an UP/RIGHT jump move will land on
Bitboard	bbJumpUpLeftEnd = 0x00777777;	//	squares that an UP/RIGHT jump move will land on
Bitboard	bbJumpDownRightEnd = 0xEEEEEE00;	//	squares that an UP/RIGHT jump move will land on
Bitboard	bbJumpDownLeftEnd = 0x77777700;	//	squares that an UP/RIGHT jump move will land on
Bitboard	bbJumpAnyDirEnd = 0x00666600;	//	squares that a jump move can land on from any direction (simply the bitwise AND of the above 4 jump masks)



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
	//	THIS IS FOR TESTING BITMASKS - comment it out for the normal playable version
	//bbP1StartPieces = 0;
	//bbP2StartPieces = bbJumpUpRightEnd;
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
	bbResult = abbSquareMasks[iBitIndex];
	return bbResult;
}

int	GetBitIndexFromXY(int iX, int iZ)
{
	return (iZ * 4) + (iX / 2);
}

int	GetBitIndex(Bitboard a_bbBoard)
{
	for (unsigned int i = 0; i < 32; ++i)
	{
		if ((a_bbBoard & abbSquareMasks[i]) > 0)
		{
			return i;
		}
	}
}

int	GetBoardXCoord(Bitboard a_bbBoard)
{
	int	iTemp = GetBitIndex(a_bbBoard);
	return GetBoardXCoordFromIndex(iTemp);
}

int	GetXOffsetFromRow(int a_iRow)
{
	return (a_iRow + 1) % 2;
}

int	GetBoardYCoord(Bitboard a_bbBoard)
{
	int	iTemp = GetBitIndex(a_bbBoard);
	return iTemp / 4;
}

int	GetBoardXCoordFromIndex(int a_iIndex)
{
	return (a_iIndex % 4) * 2 + (((a_iIndex / 4) + 1) % 2);
}

int	GetBoardYCoordFromIndex(int a_iIndex)
{
	return a_iIndex / 4;
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

Bitboard	GetCurrentAvailableMovers(GameState a_oCurrentState, bool &a_bJumpers)
{
	//	This function checks for current moves available to the player passed in.  It first check for any capture moves, and if none are
	//	found then looks for non-capture moves.
	//	It returns a bitboard containing all pieces that can make a valid move.
	//	If the current movers are jumpers, then set the input bool reference to true.
	Bitboard	bbResult = 0;	//	start with an empty bitboard
	bbResult = CheckForCaptureMoves(a_oCurrentState);
	if (bbResult > 0)
	{
		//	we have capture moves available, set the jumperse flag and return the pieces able to capture
		a_bJumpers = true;
		return bbResult;
	}
	a_bJumpers = false;
	//	No capture moves available, so check for non-capture moves
	Bitboard	bbAllPieces = a_oCurrentState.m_P1Pieces | a_oCurrentState.m_P2Pieces;
	Bitboard	bbEmptySquares = ~bbAllPieces;
	Bitboard	bbTemp;
	Bitboard	bbKings;
	if (a_oCurrentState.m_iCurrentPlayer == 1)
	{
		bbTemp = (bbEmptySquares >> 4) & a_oCurrentState.m_P1Pieces;	//	can a piece move to the main next row position
		bbResult |= bbTemp;	//	add this to the resulting moves
		//	and then we need bitmasks ...
		//	now check for non-king shift 3 and 5 moves
		bbTemp = (((bbEmptySquares >> 3) & a_oCurrentState.m_P1Pieces & bbP1L3) | ((bbEmptySquares >> 5) & a_oCurrentState.m_P1Pieces & bbP1L5));
		bbResult |= bbTemp;
		//	now add in any King moves up the board
		bbKings = a_oCurrentState.m_P1Pieces & a_oCurrentState.m_Kings;
		if (bbKings > 0)
		{
			//	only bother doing this if there are any kings
			bbTemp = (bbEmptySquares << 4) & bbKings;	//	first the main shift 4 up move
			bbResult |= bbTemp;
			bbTemp = ((bbEmptySquares << 3) & bbKings & bbP2R3) | ((bbEmptySquares << 5) & bbKings & bbP2R5);	//	now check the King +3 and +5 moves up the board
			bbResult |= bbTemp;
		}
	}
	else
	{
		//	a_iPlayer == 2
		bbTemp = (bbEmptySquares << 4) & a_oCurrentState.m_P2Pieces;	//	can a piece move to the main next row position
		bbResult |= bbTemp;
		//	now check for non-king shift 3 and 5 moves
		bbTemp = (((bbEmptySquares << 3) & a_oCurrentState.m_P2Pieces & bbP2R3) | ((bbEmptySquares << 5) & a_oCurrentState.m_P2Pieces & bbP2R5));
		bbResult |= bbTemp;
		//	now add in any King moves up the board
		bbKings = a_oCurrentState.m_P2Pieces & a_oCurrentState.m_Kings;
		if (bbKings > 0)
		{
			//	only bother doing this if there are any kings
			bbTemp = (bbEmptySquares >> 4) & bbKings;	//	first the main shift 4 up move
			bbResult |= bbTemp;
			bbTemp = ((bbEmptySquares >> 3) & bbKings & bbP1L3) | ((bbEmptySquares >> 5) & bbKings & bbP1L5);	//	now check the King +3 and +5 moves up the board
			bbResult |= bbTemp;
		}
	}

	//	Just for testing!!!!
//	return 0x00000F00;

	return bbResult;
}

Bitboard	GetPlayerJumpers(int a_iPlayer, GameState a_oCurrentState)
{
	Bitboard	bbEmpty = GetEmptySquares(a_oCurrentState.m_P1Pieces, a_oCurrentState.m_P2Pieces);
	Bitboard	bbPlayerKings;
	Bitboard	bbJumpers = 0;
	Bitboard	bbTemp = 0;
	if (a_iPlayer == 1)
	{
		//	return player 1's potential jumping pieces
		bbPlayerKings = a_oCurrentState.m_P1Pieces & a_oCurrentState.m_Kings;
		bbTemp = (bbEmpty >> 4) & a_oCurrentState.m_P2Pieces;	//	find enemy pieces with an empty jump 4 square on the other side of them
		if (bbTemp > 0)
		{
			//	first find regular men who can jump
			bbJumpers |= (((bbTemp >> 3) & bbP1L3 & a_oCurrentState.m_P1Pieces) | ((bbTemp >> 5) & bbP1L5 & a_oCurrentState.m_P1Pieces));
		}
		//	now do the same for P1 pieces that have an enemy on their +4 square ...
		//	first find enemy pieces with an empty square on their +3 or +5 squares
		bbTemp = (((bbEmpty >> 3) & a_oCurrentState.m_P2Pieces & bbP1L3) | ((bbEmpty >> 5) & a_oCurrentState.m_P2Pieces & bbP1L5));
		bbJumpers |= ((bbTemp >> 4) & a_oCurrentState.m_P1Pieces);
		//	now get the extra moves available to Kings
		if (bbPlayerKings > 0)
		{
			bbTemp = (bbEmpty << 4) & a_oCurrentState.m_P2Pieces;	//	find enemy pieces with an empty jump 4 square on the other side of them
			if (bbTemp > 0)
			{
				//	now find kings who can jump
				bbJumpers |= ((bbTemp << 3) & bbP2R3 & bbPlayerKings) | ((bbTemp << 5) & bbP2R5 & bbPlayerKings);
			}
			//	now do the same for P1 Kings that have an enemy on their -4 square ...
			bbTemp = (((bbEmpty << 3) & a_oCurrentState.m_P2Pieces & bbP2R3) | ((bbEmpty << 5) & a_oCurrentState.m_P2Pieces & bbP2R5));
			bbJumpers |= ((bbTemp << 4) & bbPlayerKings);
		}
	}
	else
	{
		//	return player 2's potential jumping pieces
		bbPlayerKings = a_oCurrentState.m_P2Pieces & a_oCurrentState.m_Kings;
		bbTemp = (bbEmpty << 4) & a_oCurrentState.m_P1Pieces;	//	find enemy pieces with an empty jump 4 square on the other side of them
		if (bbTemp > 0)
		{
			//	first find regular men who can jump
			bbJumpers |= (((bbTemp << 3) & bbP2R3 & a_oCurrentState.m_P2Pieces) | ((bbTemp << 5) & bbP2R5 & a_oCurrentState.m_P2Pieces));
		}
		//	now do the same for P1 pieces that have an enemy on their -4 square ...
		//	first find enemy pieces with an empty square on their -3 or -5 squares
		bbTemp = (((bbEmpty << 3) & a_oCurrentState.m_P1Pieces & bbP2R3) | ((bbEmpty << 5) & a_oCurrentState.m_P1Pieces & bbP2R5));
		bbJumpers |= ((bbTemp << 4) & a_oCurrentState.m_P2Pieces);
		//	now get the extra moves available to Kings
		if (bbPlayerKings > 0)
		{
			bbTemp = (bbEmpty >> 4) & a_oCurrentState.m_P1Pieces;	//	find enemy pieces with an empty jump 4 square on the other side of them
			if (bbTemp > 0)
			{
				//	now find kings who can jump
				bbJumpers |= ((bbTemp >> 3) & bbP1L3 & bbPlayerKings) | ((bbTemp >> 5) & bbP1L5 & bbPlayerKings);
			}
			//	now do the same for P2 Kings that have an enemy on their +4 square ...
			bbTemp = (((bbEmpty >> 3) & a_oCurrentState.m_P1Pieces & bbP1L3) | ((bbEmpty >> 5) & a_oCurrentState.m_P1Pieces & bbP1L5));
			bbJumpers |= ((bbTemp >> 4) & bbPlayerKings);
		}
	}
	return bbJumpers;
}

Bitboard	CheckForCaptureMoves(GameState a_oCurrentState)
{
	//	This function checks for any capture moves available for the player passed in, and returns a BitBoard containing all pieces able to
	//	capture in the current move.  If no capture moves are available the returned Bitboard will be empty (0).
	Bitboard	bbResult = 0;	//	start with an empty bitboard
	if (a_oCurrentState.m_iCurrentPlayer == 1)
	{
		bbResult = GetPlayerJumpers(1, a_oCurrentState);
	}
	else
	{
		bbResult = GetPlayerJumpers(2, a_oCurrentState);
	}

	return bbResult;
}

bool	ValidJump(Move a_oTestMove, GameState a_oCurrentState)
{
	//	at present internal to this cpp file.
	//	tests the jump passed in to see if it is valid
	Bitboard	bbEmpty = GetEmptySquares(a_oCurrentState.m_P1Pieces, a_oCurrentState.m_P2Pieces);
	bbEmpty &= a_oTestMove.EndPos;	//	we are only interested in testing the end position for a valid jump move, and making sure it is an empty square
	Bitboard	bbPlayerKing;
	Bitboard	bbJumpers = 0;
	Bitboard	bbTemp = 0;
	if (a_oCurrentState.m_iCurrentPlayer == 1)
	{
		//	test for player 1 jumps
		bbPlayerKing = a_oTestMove.StartPos & a_oCurrentState.m_Kings;
		bbTemp = (bbEmpty >> 4) & a_oCurrentState.m_P2Pieces;	//	find enemy pieces with an empty jump 4 square on the other side of them
		if (bbTemp > 0)
		{
			//	first find regular men who can jump
			bbJumpers |= (((bbTemp >> 3) & bbP1L3 & a_oTestMove.StartPos) | ((bbTemp >> 5) & bbP1L5 & a_oTestMove.StartPos));
		}
		//	now do the same for P1 pieces that have an enemy on their +4 square ...
		//	first find enemy pieces with an empty square on their +3 or +5 squares
		bbTemp = (((bbEmpty >> 3) & a_oCurrentState.m_P2Pieces & bbP1L3) | ((bbEmpty >> 5) & a_oCurrentState.m_P2Pieces & bbP1L5));
		bbJumpers |= ((bbTemp >> 4) & a_oTestMove.StartPos);
		if (bbJumpers > 0)
		{
			return true;
		}
		//	now get the extra moves available to Kings
		if (bbPlayerKing > 0)
		{
			bbTemp = (bbEmpty << 4) & a_oCurrentState.m_P2Pieces;	//	find enemy pieces with an empty jump 4 square on the other side of them
			if (bbTemp > 0)
			{
				//	now find kings who can jump
				bbJumpers |= ((bbTemp << 3) & bbP2R3 & bbPlayerKing) | ((bbTemp << 5) & bbP2R5 & bbPlayerKing);
			}
			//	now do the same for P1 Kings that have an enemy on their -4 square ...
			bbTemp = (((bbEmpty << 3) & a_oCurrentState.m_P2Pieces & bbP2R3) | ((bbEmpty << 5) & a_oCurrentState.m_P2Pieces & bbP2R5));
			bbJumpers |= ((bbTemp << 4) & bbPlayerKing);
			if (bbJumpers > 0)
			{
				return true;
			}
		}
	}
	else
	{
		//	test for player 2 jumps
		bbPlayerKing = a_oTestMove.StartPos & a_oCurrentState.m_Kings;
		bbTemp = (bbEmpty << 4) & a_oCurrentState.m_P1Pieces;	//	find enemy pieces with an empty jump 4 square on the other side of them
		if (bbTemp > 0)
		{
			//	first find regular men who can jump
			bbJumpers |= (((bbTemp << 3) & bbP2R3 & a_oTestMove.StartPos) | ((bbTemp << 5) & bbP2R5 & a_oTestMove.StartPos));
		}
		//	now do the same for P1 pieces that have an enemy on their -4 square ...
		//	first find enemy pieces with an empty square on their -3 or -5 squares
		bbTemp = (((bbEmpty << 3) & a_oCurrentState.m_P1Pieces & bbP2R3) | ((bbEmpty << 5) & a_oCurrentState.m_P1Pieces & bbP2R5));
		bbJumpers |= ((bbTemp << 4) & a_oTestMove.StartPos);
		if (bbJumpers > 0)
		{
			return true;
		}
		//	now get the extra moves available to Kings
		if (bbPlayerKing > 0)
		{
			bbTemp = (bbEmpty >> 4) & a_oCurrentState.m_P1Pieces;	//	find enemy pieces with an empty jump 4 square on the other side of them
			if (bbTemp > 0)
			{
				//	now find kings who can jump
				bbJumpers |= ((bbTemp >> 3) & bbP1L3 & bbPlayerKing) | ((bbTemp >> 5) & bbP1L5 & bbPlayerKing);
			}
			//	now do the same for P2 Kings that have an enemy on their +4 square ...
			bbTemp = (((bbEmpty >> 3) & a_oCurrentState.m_P1Pieces & bbP1L3) | ((bbEmpty >> 5) & a_oCurrentState.m_P1Pieces & bbP1L5));
			bbJumpers |= ((bbTemp >> 4) & bbPlayerKing);
			if (bbJumpers > 0)
			{
				return true;
			}
		}
	}
	return false;
}

bool	ValidMove(Move a_oTestMove, GameState a_oCurrentState)
{
	//	This function takes in a specific move and a game state and returns whether the move is a valid move or not
	//	Useful information found on this web page:
	//	http://www.3dkingdoms.com/checkers/bitboards.htm
	Bitboard	bbStartPos = a_oTestMove.StartPos;
	Bitboard	bbEndPos = a_oTestMove.EndPos;
	Bitboard	bbEmpty = GetEmptySquares(a_oCurrentState.m_P1Pieces, a_oCurrentState.m_P2Pieces);
	if ((bbEndPos & bbEmpty) == 0)
	{
		//	then the end position square is not empty, therefore this cannot be a valid move
		return false;
	}
	//	from here on we know the end position is an empty square
	//	first check for jump moves as these are forced
	if (a_oTestMove.bJumper)
	{
		//	check if the jump is valid
		return ValidJump(a_oTestMove, a_oCurrentState);
	}
	//	the move is not a jumper, so check for valid normal moves
	if (a_oCurrentState.m_iCurrentPlayer == 1)
	{
		//	check if a valid move for player 1
		//	first check if the start position is actually player 1's piece!
		if ((bbStartPos & a_oCurrentState.m_P1Pieces) == 0)
		{
			//	someone's trying to move the other player's piece!
			return false;
		}
		//	first check for valid man moves
		//	now check for the standard shift 4 "zigzag" down the board
		if (((bbStartPos << 4) & bbEndPos) > 0)
		{
			//	the move is a shift to the Shift 4 next row position
			return true;
		}
		//	now check for the shift 3 and shift 5 moves ...
		if ((((bbStartPos & bbP1L3) << 3) & bbEndPos) > 0)
		{
			//	the move is a valid move to the Shift 3 next row position
			return true;
		}
		if ((((bbStartPos & bbP1L5) << 5) & bbEndPos) > 0)
		{
			//	the move is a valid move to the Shift 5 next row position
			return true;
		}
		//	next check if this is a king
		if ((bbStartPos & a_oCurrentState.m_Kings) > 0)
		{
			//	it is a king so check for valid king moves in the other direction
			//	now check for the standard shift 4 "zigzag" up the board
			if (((bbStartPos >> 4) & bbEndPos) > 0)
			{
				//	the move is a shift to the Shift 4 next row position
				return true;
			}
			//	now check for the shift 3 and shift 5 moves ...
			if ((((bbStartPos & bbP2R3) >> 3) & bbEndPos) > 0)
			{
				//	the move is a valid move to the Shift 3 next row position
				return true;
			}
			if ((((bbStartPos & bbP2R5) >> 5) & bbEndPos) > 0)
			{
				//	the move is a valid move to the Shift 5 next row position
				return true;
			}
		}
	}
	else
	{
		//	check if a valid move for player 2
		//	first check if the start position is actually player 1's piece!
		if ((bbStartPos & a_oCurrentState.m_P2Pieces) == 0)
		{
			//	someone's trying to move the other player's piece!
			return false;
		}
		//	first check for valid man moves
		//	now check for the standard shift 4 "zigzag" up the board
		if (((bbStartPos >> 4) & bbEndPos) > 0)
		{
			//	the move is a shift to the Shift 4 next row position
			return true;
		}
		//	now check for the shift 3 and shift 5 moves ...
		if ((((bbStartPos & bbP2R3) >> 3) & bbEndPos) > 0)
		{
			//	the move is a valid move to the Shift 3 next row position
			return true;
		}
		if ((((bbStartPos & bbP2R5) >> 5) & bbEndPos) > 0)
		{
			//	the move is a valid move to the Shift 5 next row position
			return true;
		}
		//	next check if this is a king
		if ((bbStartPos & a_oCurrentState.m_Kings) > 0)
		{
			//	it is a king so check for valid king moves in the other direction
			//	now check for the standard shift 4 "zigzag" down the board
			if (((bbStartPos << 4) & bbEndPos) > 0)
			{
				//	the move is a shift to the Shift 4 next row position
				return true;
			}
			//	now check for the shift 3 and shift 5 moves ...
			if ((((bbStartPos & bbP1L3) << 3) & bbEndPos) > 0)
			{
				//	the move is a valid move to the Shift 3 next row position
				return true;
			}
			if ((((bbStartPos & bbP1L5) << 5) & bbEndPos) > 0)
			{
				//	the move is a valid move to the Shift 5 next row position
				return true;
			}
		}
	}
	return false;
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
		//if ((a_bbBoard & (0x00000001 << i)) > 0)
		if ((a_bbBoard & abbSquareMasks[i]) > 0)
		{
			++iResult;
		}
	}

	return iResult;
}

bool	IsCaptureMove(Move a_oMove)
{
	//	basically need to get what bit the start and end positions are in and if the difference is greater than 6 then it was a capture move
	//	This is pretty much deprecated by the bool jumper flag in the move struct itself now.
	int	iStartIndex = GetFirstBitNumber(a_oMove.StartPos);
	int	iEndIndex = GetFirstBitNumber(a_oMove.EndPos);

	if (abs(iStartIndex - iEndIndex) > 6)
		return true;
	else
		return false;
}

void	GenerateMovesForMover(int a_iMover, GameState a_oGameState, std::vector<Move> &a_aMoveList, bool a_bJumper)
{
	//	This function generates available moves for the piece at the index passed in, using the game state passed in, and adds them to the vector of moves passed in.
	Move	oTempMove;
	oTempMove.bJumper = a_bJumper;
	oTempMove.StartPos = abbSquareMasks[a_iMover];
	//	and this is where I diverge from pure bitboards to make move generation simpler ... primarily because I simply need to get stuff working right about now!
	int	iStartX = GetBoardXCoordFromIndex(a_iMover);
	int	iStartY = GetBoardYCoordFromIndex(a_iMover);
	int	iRowOffset;
	Bitboard	bbMover = abbSquareMasks[a_iMover];
	Bitboard	bbEmptySquares = GetEmptySquares(a_oGameState.m_P1Pieces, a_oGameState.m_P2Pieces);
	Bitboard	bbEndSquare;
	if (a_oGameState.m_iCurrentPlayer == 1)
	{
		if ((bbMover & a_oGameState.m_P1Pieces) == 0)
		{
			//	the mover passed in does not belong to the current player, so this is not valid - do not generate any moves
			return;
		}
		if (a_bJumper)
		{
			//	then only generate Player 1 jump moves ...
			//	first look at valid man jumps
			if (iStartY < 6)
			{
				//	we have a potential forward jump
				//	first test a jump down/right
				if (iStartX < 6)
				{
					bbEndSquare = abbSquareMasks[GetBitIndexFromXY(iStartX + 2, iStartY + 2)];
					if (((abbSquareMasks[GetBitIndexFromXY(iStartX + 1, iStartY + 1)] & a_oGameState.m_P2Pieces) > 0) && ((bbEndSquare & bbEmptySquares) > 0))
					{
						//	then we have a valid jump down/right
						oTempMove.EndPos = bbEndSquare;
						a_aMoveList.push_back(oTempMove);
					}
				}
				//	now test a jump down/left
				if (iStartX > 1)
				{
					bbEndSquare = abbSquareMasks[GetBitIndexFromXY(iStartX - 2, iStartY + 2)];
					if (((abbSquareMasks[GetBitIndexFromXY(iStartX - 1, iStartY + 1)] & a_oGameState.m_P2Pieces) > 0) && ((bbEndSquare & bbEmptySquares) > 0))
					{
						//	then we have a valid jump down/left
						oTempMove.EndPos = bbEndSquare;
						a_aMoveList.push_back(oTempMove);
					}
				}
			}
			//	now look at valid extra King jumps if this is a King
			if ((bbMover & a_oGameState.m_Kings) > 0)
			{
				//	we have a King, so check its extra jumps up the board
				if (iStartY > 1)
				{
					//	we have a potential upward jump
					//	first test a jump up/right
					if (iStartX < 6)
					{
						bbEndSquare = abbSquareMasks[GetBitIndexFromXY(iStartX + 2, iStartY - 2)];
						if (((abbSquareMasks[GetBitIndexFromXY(iStartX + 1, iStartY - 1)] & a_oGameState.m_P2Pieces) > 0) && ((bbEndSquare & bbEmptySquares) > 0))
						{
							//	then we have a valid jump up/right
							oTempMove.EndPos = bbEndSquare;
							a_aMoveList.push_back(oTempMove);
						}
					}
					//	now test a jump up/left
					if (iStartX > 1)
					{
						bbEndSquare = abbSquareMasks[GetBitIndexFromXY(iStartX - 2, iStartY - 2)];
						if (((abbSquareMasks[GetBitIndexFromXY(iStartX - 1, iStartY - 1)] & a_oGameState.m_P2Pieces) > 0) && ((bbEndSquare & bbEmptySquares) > 0))
						{
							//	then we have a valid jump up/left
							oTempMove.EndPos = bbEndSquare;
							a_aMoveList.push_back(oTempMove);
						}
					}
				}
			}
		}
		else
		{
			//	generate legal Player 1 non-jump moves
			//	first look at valid man moves
			if (iStartY < 7)
			{
				//	we have a potential forward move
				//	first test a move down/right
				if (iStartX < 7)
				{
					bbEndSquare = abbSquareMasks[GetBitIndexFromXY(iStartX + 1, iStartY + 1)];
					if ((bbEndSquare & bbEmptySquares) > 0)
					{
						//	then we have a valid move down/right
						oTempMove.EndPos = bbEndSquare;
						a_aMoveList.push_back(oTempMove);
					}
				}
				//	now test a move down/left
				if (iStartX > 0)
				{
					bbEndSquare = abbSquareMasks[GetBitIndexFromXY(iStartX - 1, iStartY + 1)];
					if ((bbEndSquare & bbEmptySquares) > 0)
					{
						//	then we have a valid move down/left
						oTempMove.EndPos = bbEndSquare;
						a_aMoveList.push_back(oTempMove);
					}
				}
			}
			//	now look at valid extra King moves if this is a King
			if ((bbMover & a_oGameState.m_Kings) > 0)
			{
				//	we have a King, so check its extra moves up the board
				if (iStartY > 0)
				{
					//	we have a potential upward move
					//	first test a move up/right
					if (iStartX < 7)
					{
						bbEndSquare = abbSquareMasks[GetBitIndexFromXY(iStartX + 1, iStartY - 1)];
						if ((bbEndSquare & bbEmptySquares) > 0)
						{
							//	then we have a valid move up/right
							oTempMove.EndPos = bbEndSquare;
							a_aMoveList.push_back(oTempMove);
						}
					}
					//	now test a move up/left
					if (iStartX > 0)
					{
						bbEndSquare = abbSquareMasks[GetBitIndexFromXY(iStartX - 1, iStartY - 1)];
						if ((bbEndSquare & bbEmptySquares) > 0)
						{
							//	then we have a valid move up/left
							oTempMove.EndPos = bbEndSquare;
							a_aMoveList.push_back(oTempMove);
						}
					}
				}
			}
		}
	}
	else
	{
		//	generate legal Player 2 moves
		if ((abbSquareMasks[a_iMover] & a_oGameState.m_P2Pieces) == 0)
		{
			//	the mover passed in does not belong to the current player, so this is not valid - do not generate any moves
			return;
		}
		if (a_bJumper)
		{
			//	then only generate Player 2 jump moves ...
			//	first look at valid man jumps
			if (iStartY > 1)
			{
				//	we have a potential forward jump
				//	first test a jump up/right
				if (iStartX < 6)
				{
					bbEndSquare = abbSquareMasks[GetBitIndexFromXY(iStartX + 2, iStartY - 2)];
					if (((abbSquareMasks[GetBitIndexFromXY(iStartX + 1, iStartY - 1)] & a_oGameState.m_P1Pieces) > 0) && ((bbEndSquare & bbEmptySquares) > 0))
					{
						//	then we have a valid jump up/right
						oTempMove.EndPos = bbEndSquare;
						a_aMoveList.push_back(oTempMove);
					}
				}
				//	now test a jump up/left
				if (iStartX > 1)
				{
					bbEndSquare = abbSquareMasks[GetBitIndexFromXY(iStartX - 2, iStartY - 2)];
					if (((abbSquareMasks[GetBitIndexFromXY(iStartX - 1, iStartY - 1)] & a_oGameState.m_P1Pieces) > 0) && ((bbEndSquare & bbEmptySquares) > 0))
					{
						//	then we have a valid jump up/left
						oTempMove.EndPos = bbEndSquare;
						a_aMoveList.push_back(oTempMove);
					}
				}
			}
			//	now look at valid extra King jumps if this is a King
			if ((bbMover & a_oGameState.m_Kings) > 0)
			{
				//	we have a King, so check its extra jumps down the board
				if (iStartY < 6)
				{
					//	we have a potential downward jump
					//	first test a jump down/right
					if (iStartX < 6)
					{
						bbEndSquare = abbSquareMasks[GetBitIndexFromXY(iStartX + 2, iStartY + 2)];
						if (((abbSquareMasks[GetBitIndexFromXY(iStartX + 1, iStartY + 1)] & a_oGameState.m_P1Pieces) > 0) && ((bbEndSquare & bbEmptySquares) > 0))
						{
							//	then we have a valid jump down/right
							oTempMove.EndPos = bbEndSquare;
							a_aMoveList.push_back(oTempMove);
						}
					}
					//	now test a jump down/left
					if (iStartX > 1)
					{
						bbEndSquare = abbSquareMasks[GetBitIndexFromXY(iStartX - 2, iStartY + 2)];
						if (((abbSquareMasks[GetBitIndexFromXY(iStartX - 1, iStartY + 1)] & a_oGameState.m_P1Pieces) > 0) && ((bbEndSquare & bbEmptySquares) > 0))
						{
							//	then we have a valid jump up/left
							oTempMove.EndPos = bbEndSquare;
							a_aMoveList.push_back(oTempMove);
						}
					}
				}
			}
		}
		else
		{
			//	generate legal Player 2 non-jump moves
			//	first look at valid man moves
			if (iStartY > 0)
			{
				//	we have a potential forward move
				//	first test a move up/right
				if (iStartX < 7)
				{
					bbEndSquare = abbSquareMasks[GetBitIndexFromXY(iStartX + 1, iStartY - 1)];
					if ((bbEndSquare & bbEmptySquares) > 0)
					{
						//	then we have a valid move up/right
						oTempMove.EndPos = bbEndSquare;
						a_aMoveList.push_back(oTempMove);
					}
				}
				//	now test a move up/left
				if (iStartX > 0)
				{
					bbEndSquare = abbSquareMasks[GetBitIndexFromXY(iStartX - 1, iStartY - 1)];
					if ((bbEndSquare & bbEmptySquares) > 0)
					{
						//	then we have a valid move up/left
						oTempMove.EndPos = bbEndSquare;
						a_aMoveList.push_back(oTempMove);
					}
				}
			}
			//	now look at valid extra King moves if this is a King
			if ((bbMover & a_oGameState.m_Kings) > 0)
			{
				//	we have a King, so check its extra moves down the board
				if (iStartY < 7)
				{
					//	we have a potential downward move
					//	first test a move down/right
					if (iStartX < 7)
					{
						bbEndSquare = abbSquareMasks[GetBitIndexFromXY(iStartX + 1, iStartY + 1)];
						if ((bbEndSquare & bbEmptySquares) > 0)
						{
							//	then we have a valid move down/right
							oTempMove.EndPos = bbEndSquare;
							a_aMoveList.push_back(oTempMove);
						}
					}
					//	now test a move down/left
					if (iStartX > 0)
					{
						bbEndSquare = abbSquareMasks[GetBitIndexFromXY(iStartX - 1, iStartY + 1)];
						if ((bbEndSquare & bbEmptySquares) > 0)
						{
							//	then we have a valid move down/left
							oTempMove.EndPos = bbEndSquare;
							a_aMoveList.push_back(oTempMove);
						}
					}
				}
			}
		}
	}
}
