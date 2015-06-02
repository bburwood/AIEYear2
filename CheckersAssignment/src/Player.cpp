#include "Player.h"
#include "Game.h"

Player::Player()
{
}

Player::~Player()
{
}

void	Player::InitPlayer(int a_iPlayerNumber, PlayerType a_ePlayerType, float a_fMaxTimePerAIMove, Game* a_pGame)
{
	m_iPlayerNumber = a_iPlayerNumber;
	m_ePlayerType = a_ePlayerType;
	m_fMaxTimePerAIMove = a_fMaxTimePerAIMove;
	m_pGame = a_pGame;
	m_bNotMyTurn = true;
}

void	Player::update(float dT, GameState a_oCurrentGameState)
{
	if (a_oCurrentGameState.m_iCurrentPlayer == m_iPlayerNumber)
	{
		if (m_ePlayerType == PLAYER_MCTS_AI)
		{
			if (m_bNotMyTurn)
			{
				//	it has just become my turn, so start deciding what move to make
				//	this is where I would spawn off the decision thread ...
				m_bNotMyTurn = false;
				MakeMoveDecision();
			}
			m_fCurrentMoveTimer += dT;
		}
		else
		{
			//	Human player, so monitor the mouse clicks to get the moves
			if (m_bNotMyTurn)
			{
				//	it has just become my turn, so set relevant flags
				m_bNotMyTurn = false;
				m_bPieceSelected = false;
				m_bMoveCompleted = false;
				//	clear any old move information
				m_oSelectedMove.StartPos = 0;
				m_oSelectedMove.EndPos = 0;
			}
			if ((m_iMouseX != -1) && (m_iMouseY != -1))
			{
				//	then the mouse has been clicked somewhere on the board
				Bitboard	bbMouseClick = GenerateBitMaskFromCoords(m_iMouseX, m_iMouseY);
				Bitboard	bbAvailableMoves = GetCurrentAvailableMoves(a_oCurrentGameState.m_iCurrentPlayer, a_oCurrentGameState.m_P1Pieces, a_oCurrentGameState.m_P1Kings, a_oCurrentGameState.m_P2Pieces, a_oCurrentGameState.m_P2Kings);
				if ((bbMouseClick & bbAvailableMoves) > 0)
				{
					//	then we have a click on a piece with a valid move
					m_oSelectedMove.StartPos = bbMouseClick;
				}
				if (m_oSelectedMove.StartPos > 0)
				{
					//	we have a valid start move recorded, check if this is a valid empty square to move to
					Bitboard	bbEmptySquares = GetEmptySquares(a_oCurrentGameState.m_P1Pieces, a_oCurrentGameState.m_P2Pieces);
					if ((bbMouseClick & bbEmptySquares) > 0)
					{
						//	an empty square has been clicked on, so check if this will be a valid move
						Move	oTestMove;
						oTestMove.StartPos = m_oSelectedMove.StartPos;
						oTestMove.EndPos = bbMouseClick;
						if (ValidMove(oTestMove, a_oCurrentGameState))
						{
							//	then we have a valid move, so make it!
							m_pGame->MakeMove(oTestMove);
						}
					}
				}
			}
		}

	}
	else
	{
		m_bNotMyTurn = true;
	}
}

float	Player::ScoreCurrentBoard(int a_iPlayer, Bitboard a_bbP1Pieces, Bitboard a_bbP1Kings, Bitboard a_bbP2Pieces, Bitboard a_bbP2Kings)
{
	//	This function scores the board for the player passed in.  It is a static evaluation of the current board state, and does not
	//	take into account future moves (except possible captures for the current player in the current turn).
	//	Improvements to this function will also likely yield decent improvements to the AI decision making process.
	float	fResult = 0.0f;	//	start with a zero score

	//	For the initial version of this function the score will simply be a function of the number of pieces and kings a player has,
	//	as compared to the other player.
	//	Initially it will score 1 for a regular piece, and 10 for a King, eventually I will also add in that instead of using 1 for a regular
	//	piece, it will use how many rows advanced along the board that piece is. (hence the need for something bigger than 7 for a King)
	float	fP1Pieces = (float)CountPieces(a_bbP1Pieces);
	float	fP1Kings = (float)CountPieces(a_bbP1Kings);
	float	fP2Pieces = (float)CountPieces(a_bbP2Pieces);
	float	fP2Kings = (float)CountPieces(a_bbP2Kings);

	//	The following 2 scoring components should be NON-ZERO by definition, while there are still pieces on the board.
	//	This is where the row of the regular pieces will eventually need to be taken into account.
	float	fP1Term = (fP1Pieces - fP1Kings) + (10.0f * fP1Kings);
	float	fP2Term = (fP2Pieces - fP2Kings) + (10.0f * fP2Kings);

	if (a_iPlayer == 1)
	{
		fResult = fP1Term / fP2Term;
	}
	else
	{
		fResult = fP2Term / fP1Term;
	}

	return fResult;
}

void	Player::MakeMoveDecision()
{

}

void	Player::MouseClickedOnBoardAt(int iBoardX, int iBoardY)
{
	//	set values based on what is passed through
	m_iMouseX = iBoardX;
	m_iMouseY = iBoardY;
}

bool	Player::ValidMove(Move a_oTestMove, GameState a_oCurrentState)
{
	//	This function takes in a move and a game state and returns whether the move is a valid move or not
	//	Useful information found on this web page:
	//	http://www.3dkingdoms.com/checkers/bitboards.htm
	bool	bResult = false;

	if (a_oCurrentState.m_iCurrentPlayer == 1)
	{
		//	check if a valid move for player 1
		//	first check if this is a king
		if ((a_oTestMove.StartPos & a_oCurrentState.m_P1Kings) > 1)
		{
			//	it is a king so check for valid king moves
		}
		else
		{
			//	it is not a king, so check for valid man moves
		}
	}
	else
	{
		//	check if a valid move for player 2
	}

	return true;	//	just for testing ...

//	return bResult;
}
