#include "Player.h"
#include "Game.h"
#include "Checkers.h"	//	again ... ONLY used for telling the program to fire off the particle emitters for a capture move when made ...
						//	I should probably move them into the Game class instead ...

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
		//	if it is my turn then mago through the turn taking logic
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
				m_bMoveInProgress = false;
				m_bMoveCompleted = false;
				//	clear any old move information
				m_oSelectedMove.StartPos = 0;
				m_oSelectedMove.EndPos = 0;
				m_oSelectedMove.bJumper = false;
			}
			if ((m_iMouseX != -1) && (m_iMouseY != -1))
			{
				//	we have clicked on the board
				bool	bJumpers = false;
				Bitboard	bbMouseClick = GenerateBitMaskFromCoords(m_iMouseX, m_iMouseY);
				Bitboard	bbAvailableMoves;
				if (m_bMoveInProgress)
				{
					bbAvailableMoves = m_oSelectedMove.StartPos;
					bJumpers = true;
				}
				else
				{
					bbAvailableMoves = GetCurrentAvailableMovers(a_oCurrentGameState, bJumpers);
				}
				GenerateMovesFromAvailableMovers(bbAvailableMoves, bJumpers);
				if (m_pGame->m_aMoveList.size() == 0)
				{
					//	no moves were generated so end the turn
					m_bMoveCompleted = true;
				}
				if (!m_bMoveCompleted)
				{
					if (((bbMouseClick & bbAvailableMoves) > 0) && !m_bMoveInProgress)
					{
						//	then we have a click on a piece with a valid move
						m_oSelectedMove.StartPos = bbMouseClick;
						m_bPieceSelected = true;
						//	then fire off a selected emitter
						int	iFireX = (GetBoardXCoord(bbMouseClick) + GetBoardXCoord(bbMouseClick)) / 2;
						int	iFireY = (GetBoardYCoord(bbMouseClick) + GetBoardYCoord(bbMouseClick)) / 2;
						m_pProgram->FireEmitterAt(iFireX, iFireY, 0.35f);
					}
					else if (m_bMoveInProgress)
					{
						//	check that the move can continue.  If it can't then end the turn.
						//Bitboard	bbCaptureMoves = GetPlayerJumpers(m_iPlayerNumber, a_oCurrentGameState);
						//if ((m_oSelectedMove.StartPos & bbCaptureMoves) == 0)
						if ((m_oSelectedMove.StartPos & bbAvailableMoves) == 0)
						{
							//	this move is not in the list of available moves for the player, therefore the move cannot continue
							m_bMoveInProgress = false;
							m_bMoveCompleted = true;
						}
						else
						{
							//	check if the mouse click was on an appropriate end move square

						}
					}
					//else
					if (m_bPieceSelected && (m_oSelectedMove.StartPos > 0) && (bbMouseClick != m_oSelectedMove.StartPos))
					{
						//	we have a valid start move recorded, and the mouse has clicked somewhere else, check if this is a valid empty square to move to
						Bitboard	bbEmptySquares = GetEmptySquares(a_oCurrentGameState.m_P1Pieces, a_oCurrentGameState.m_P2Pieces);
						if ((bbMouseClick & bbEmptySquares) > 0)
						{
							//	an empty square has been clicked on, so check if this will be a valid move
							Move	oTestMove;
							oTestMove.StartPos = m_oSelectedMove.StartPos;
							oTestMove.EndPos = bbMouseClick;
							oTestMove.bJumper = bJumpers;
							if (ValidMove(oTestMove, a_oCurrentGameState))
							{
								//	then we have a valid move, so make it!
								m_pGame->MakeMove(oTestMove);
								if (IsCaptureMove(oTestMove))
								{
									m_oSelectedMove.StartPos = oTestMove.EndPos;
									m_bMoveInProgress = true;
									m_bPieceSelected = true;
									//	then fire off a capture emitter
									int	iFireX = (GetBoardXCoord(oTestMove.StartPos) + GetBoardXCoord(oTestMove.EndPos)) / 2;
									int	iFireY = (GetBoardYCoord(oTestMove.StartPos) + GetBoardYCoord(oTestMove.EndPos)) / 2;
									m_pProgram->FireCaptureEmitterAt(iFireX, iFireY, 0.35f);
									m_pGame->RemoveCapturedPiece(GenerateBitMaskFromCoords(iFireX, iFireY));	//	now tell the game to remove the piece captured ...
								}
								else
								{
									m_bMoveInProgress = false;
									m_bMoveCompleted = true;
								}
								//	now need to check if the moving piece should be made a King
								//	first get the relevant King row mask ...
								Bitboard	bbKingRow;
								if (m_iPlayerNumber == 1)
								{
									bbKingRow = bbP1KingRow;
								}
								else
								{
									bbKingRow = bbP2KingRow;
								}
								if (oTestMove.EndPos & bbKingRow)
								{
									m_pGame->m_oGameState.m_Kings |= oTestMove.EndPos;
								}
							}
						}
						else
						{
							//	mouse clicked on a square with a piece in it
						}
					}
				}
				else
				{
					//	move is complete, so do nothing
				}
			}
			else
			{
				//	did not click on the board
				if (m_bMoveInProgress)
				{
					//	continue capture move from current selected startpos
					//	only need to test if the current piece can jump
					//	(do not automatically select the jump, as there may be more than 1 option - use the mouse click as the selection)

				}
			}
			if (m_bMoveCompleted)
			{
				m_bNotMyTurn = true;
				m_pGame->EndTurn();
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
	//	AI function to look for a move ...
}

void	Player::MouseClickedOnBoardAt(int iBoardX, int iBoardY)
{
	//	set values based on what is passed through
	m_iMouseX = iBoardX;
	m_iMouseY = iBoardY;
}


void	Player::GetSelectedMoveDetails(bool& a_bPieceSelected, int& a_iXCoord, int& a_iZCoord)
{
	a_bPieceSelected = m_bPieceSelected;
	a_iXCoord = GetBoardXCoord(m_oSelectedMove.StartPos);
	a_iZCoord = GetBoardYCoord(m_oSelectedMove.StartPos);
}

void	Player::SetCheckersPointer(Checkers* a_pCheckers)
{
	m_pProgram = a_pCheckers;
}

void	Player::GenerateMovesFromAvailableMovers(Bitboard a_bbMovers, bool a_bJumpers)
{
	//	This function generates a list of available moves from the bitboard of available moving pieces passed in.
	//	Also passed in is a flag indicating if the movers passed in are jumpers or not.  This simplifies the move generation requirements.
	//	The list is stored in the Game's m_aMoveList vector, replacing any moves previously stored in it.
	m_pGame->m_aMoveList.clear();
	Move	oTempMove;
	for (unsigned int i = 0; i < 32; ++i)
	{
		if ((a_bbMovers & abbSquareMasks[i]) > 0)
		{
			//	we have a moving piece, so generate any possible moves for that piece and store them in the move list vector
			//oTempMove.StartPos = abbSquareMasks[i];
			GenerateMovesForMover(i, m_pGame->m_oGameState, m_pGame->m_aMoveList, a_bJumpers);
		}
	}
}
