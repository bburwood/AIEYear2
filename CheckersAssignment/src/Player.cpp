#include "Player.h"
#include "Game.h"
#include "Checkers.h"	//	again ... ONLY used for telling the program to fire off the particle emitters for a capture move when made ...
						//	I should probably move them into the Game class instead ...
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include <iostream>
#include <stdlib.h>     /* srand, rand */

Player::Player()
{
	m_iAIThreads = 1;
	m_iThreadsForThisTurn = m_iAIThreads;
	m_iAILookAheadMoves = 50;
	m_fMaxTimePerAIMove = 1.0f;
	m_iLookAheadThisTurn = m_iAILookAheadMoves;
}

Player::~Player()
{
	CloseThreads();
}

void	Player::InitPlayer(int a_iPlayerNumber, PlayerType a_ePlayerType, float a_fMaxTimePerAIMove, Game* a_pGame)
{
	m_iPlayerNumber = a_iPlayerNumber;
	m_ePlayerType = a_ePlayerType;
	m_fMaxTimePerAIMove = a_fMaxTimePerAIMove;
	m_pGame = a_pGame;
	m_bNotMyTurn = true;
	m_bAIDecidingMove = false;
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
				m_bPieceSelected = false;
				m_bMoveInProgress = false;
				m_bMoveCompleted = false;
				m_bAIDecidingMove = true;
				m_fCurrentMoveTimer = 0.0f;
				m_iThreadsForThisTurn = m_iAIThreads;
				m_iLookAheadThisTurn = m_iAILookAheadMoves;
				BeginMoveDecision();
			}
			else if (m_bAIDecidingMove && (m_fCurrentMoveTimer < m_fMaxTimePerAIMove))
			{
				m_fCurrentMoveTimer += dT;
				//CheckMoveDecisionStatus();
			}
			else
			{
				//	first check if a move is still in progress
				if (m_bMoveInProgress)
				{
					Bitboard	bbJumpers = GetPlayerJumpers(a_oCurrentGameState.m_iCurrentPlayer, a_oCurrentGameState) & m_oSelectedMove.StartPos;
					if (bbJumpers == 0)
					{
						//	then there are no further jumps, so the turn is over, so set the end turn flag and return
						m_bMoveInProgress = false;
						m_bMoveCompleted = true;
						return;
					}
					else
					{
						AIGenerateMovesFromAvailableMovers(a_oCurrentGameState, bbJumpers, true, m_aMoveList);
						m_oSelectedMove.bJumper = true;
						if (m_aMoveList.size() == 1)
						{
							//	there is only one further jump at this time, so make it
							m_oSelectedMove.EndPos = m_aMoveList[0].EndPos;
						}
						else
						{
							//	there are more than 1 possible jumps for this piece's continuing jump, so pick a random one!
							m_oSelectedMove.EndPos = m_aMoveList[rand() % m_aMoveList.size()].EndPos;
						}
					}
				}
				else
				{
					//	Either time's up or the move has been decided.  Either way make a final call to the Check scores and set the final selected move.
					if (m_aMoveList.size() > 1)
					{
						CheckScores();
					}
				}
				//	the move has been decided so let the game know
				m_pGame->MakeMove(m_oSelectedMove);

				//	fire off emitters at the starting and ending positions, so it is easier to tell what moves were made
				int	iFireX = GetBoardXCoord(m_oSelectedMove.StartPos);
				int	iFireY = GetBoardYCoord(m_oSelectedMove.StartPos);
				m_pProgram->FireEmitterAt(iFireX, iFireY, 0.35f);
				iFireX = GetBoardXCoord(m_oSelectedMove.EndPos);
				iFireY = GetBoardYCoord(m_oSelectedMove.EndPos);
				m_pProgram->FireEmitterAt(iFireX, iFireY, 0.35f);
				if (IsCaptureMove(m_oSelectedMove))
				{
					//	then fire off a capture emitter
					iFireX = (GetBoardXCoord(m_oSelectedMove.StartPos) + GetBoardXCoord(m_oSelectedMove.EndPos)) / 2;
					iFireY = (GetBoardYCoord(m_oSelectedMove.StartPos) + GetBoardYCoord(m_oSelectedMove.EndPos)) / 2;
					m_pProgram->FireCaptureEmitterAt(iFireX, iFireY, 0.35f);
					m_pGame->RemoveCapturedPiece(GenerateBitMaskFromCoords(iFireX, iFireY));	//	now tell the game to remove the piece captured ...
					//	set the new start position
					m_oSelectedMove.StartPos = m_oSelectedMove.EndPos;
					m_bMoveInProgress = true;
					m_bPieceSelected = true;
					//m_bNotMyTurn = true;	//	this forces the AI to start looking for a new move in the next update, where BeginMoveDecision() will make a check for no available moves and end the turn.
					//	this would be the place to join the previous threads, and then fire off new threads for the new decision
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
				if ((m_oSelectedMove.EndPos & bbKingRow) > 0)
				{
					m_pGame->m_oGameState.m_Kings |= m_oSelectedMove.EndPos;
				}

			}
			if (m_bMoveCompleted)
			{
				m_bNotMyTurn = true;
				m_pGame->EndTurn();
			}
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
			if ((m_iMouseX != -1) && (m_iMouseY != -1))
			{
				//	we have clicked on the board
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
								if (oTestMove.bJumper)
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
								if ((oTestMove.EndPos & bbKingRow) > 0)
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
			//else
			//{
			//	//	did not click on the board
			//	if (m_bMoveInProgress)
			//	{
			//		//	continue capture move from current selected startpos
			//		//	only need to test if the current piece can jump
			//		//	(do not automatically select the jump, as there may be more than 1 option - use the mouse click as the selection)
			//
			//	}
			//}
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

float	Player::ScoreCurrentBoard(int a_iPlayer, GameState a_oGameState)
{
	//	This function scores the board for the player passed in.  It is a static evaluation of the current board state, and does not
	//	take into account future moves (except possible captures for the current player in the current turn).
	//	Improvements to this function will also likely yield decent improvements to the AI decision making process.
	float	fResult = 0.0f;	//	start with a zero score

	//	For the initial version of this function the score will simply be a function of the number of pieces and kings a player has,
	//	as compared to the other player.
	//	Initially it will score 1 for a regular piece, and 10 for a King, eventually I will also add in that instead of using 1 for a regular
	//	piece, it will use how many rows advanced along the board that piece is. (hence the need for something bigger than 7 for a King)
	Bitboard	bbP1Kings = a_oGameState.m_P1Pieces & a_oGameState.m_Kings;
	Bitboard	bbP1Pieces = a_oGameState.m_P1Pieces & ~bbP1Kings;
	Bitboard	bbP2Kings = a_oGameState.m_P2Pieces & a_oGameState.m_Kings;
	Bitboard	bbP2Pieces = a_oGameState.m_P2Pieces & ~bbP2Kings;
	float	fP1Pieces = 0.0f;
	float	fP1Kings = (float)CountPieces(bbP1Kings);
	float	fP2Pieces = (float)CountPieces(bbP2Pieces);
	float	fP2Kings = (float)CountPieces(bbP2Kings);
	float	fP1KingsInMiddle = (float)CountPieces(bbP1Kings & bbCentreSquares);
	float	fP2KingsInMiddle = (float)CountPieces(bbP2Kings & bbCentreSquares);
	float	fP1KingsAroundEdge = (float)CountPieces(bbP1Kings & bbAllEdgePlaces);
	float	fP2KingsAroundEdge = (float)CountPieces(bbP2Kings & bbAllEdgePlaces);
	float	fP1Jumpers = (float)CountPieces(GetPlayerJumpers(1, a_oGameState));
	float	fP2Jumpers = (float)CountPieces(GetPlayerJumpers(2, a_oGameState));

	//	The following 2 scoring components should be NON-ZERO by definition, while there are still pieces on the board.
	//	This is where the row of the regular pieces is taken into account.
	fP1Pieces += (float)CountPieces(bbP1Pieces & bbRow0) * 1.0f
		+ (float)CountPieces(bbP1Pieces & bbRow1) * 2.0f
		+ (float)CountPieces(bbP1Pieces & bbRow2) * 3.0f
		+ (float)CountPieces(bbP1Pieces & bbRow3) * 4.0f
		+ (float)CountPieces(bbP1Pieces & bbRow4) * 5.0f
		+ (float)CountPieces(bbP1Pieces & bbRow5) * 6.0f
		+ (float)CountPieces(bbP1Pieces & bbRow6) * 7.0f
		+ (float)CountPieces(bbP1Pieces & bbRow7) * 8.0f;	//	technically this is always zero, as pieces are made Kings in this row

	fP2Pieces += (float)CountPieces(bbP2Pieces & bbRow0) * 8.0f	//	technically this is always zero, as pieces are made Kings in this row
		+ (float)CountPieces(bbP2Pieces & bbRow1) * 7.0f
		+ (float)CountPieces(bbP2Pieces & bbRow2) * 6.0f
		+ (float)CountPieces(bbP2Pieces & bbRow3) * 5.0f
		+ (float)CountPieces(bbP2Pieces & bbRow4) * 4.0f
		+ (float)CountPieces(bbP2Pieces & bbRow5) * 3.0f
		+ (float)CountPieces(bbP2Pieces & bbRow6) * 2.0f
		+ (float)CountPieces(bbP2Pieces & bbRow7) * 1.0f;

	//	Bonus points: 10 points for Kings, 10 points for jumpers, and a 5 points for Kings in the middle of the board,
	//	Penalty points: 1.5 points for Kings on the edges, 0.5 points for opposition jumpers.
	float	fP1Term = fP1Pieces + (10.0f * fP1Kings) + (5.0f * fP1KingsInMiddle) + (10.0f * fP1Jumpers) - (0.5f * fP1KingsAroundEdge) - (1.5f * fP2Jumpers);
	float	fP2Term = fP2Pieces + (10.0f * fP2Kings) + (5.0f * fP2KingsInMiddle) + (10.0f * fP2Jumpers) - (0.5f * fP2KingsAroundEdge) - (1.5f * fP1Jumpers);

	if (a_iPlayer == 1)
	{
		fResult = fP1Term / fP2Term;
	}
	else
	{
		fResult = fP2Term / fP1Term;
	}

	return fResult * 0.0001f;	//	reduce the score as I was getting some strange infinite scores!
}

void	Player::BeginMoveDecision()
{
	//	AI function to kick off the search for a move ...
	//	This is where the thread(s) will be initiated ...
	m_bAITimesUp = false;
	bool	bJumpers;
	m_bbAICurrentAvailableMovers = GetCurrentAvailableMovers(m_pGame->m_oGameState, bJumpers);
	GameState	oGameState = m_pGame->m_oGameState;
	AIGenerateMovesFromAvailableMovers(oGameState, m_bbAICurrentAvailableMovers, bJumpers, m_aMoveList);	//	store the initial move list in the AI's move list vector, so we have it for reference later.
	if (m_aMoveList.size() == 1)
	{
		//	if there is only 1 move, then don't bother deciding anything just set it as the move and return
		m_oSelectedMove = m_aMoveList[0];
		m_bAIDecidingMove = false;
		return;
	}
	else if (m_aMoveList.size() == 0)
	{
		//	there are no moves available!! so end the turn here and get the game to run a GameOver check!
		m_pGame->EndTurn();
		return;
	}
	else if (m_aMoveList.size() < m_iThreadsForThisTurn)
	{
		//	then there are only a smallnumber of moves to check, so reduce the number of threads to match the number of moves
		m_iThreadsForThisTurn = m_aMoveList.size();
	}
	m_aMoveScores.clear();
	m_aPlayouts.clear();
	m_aMoveScores.reserve(m_aMoveList.size());	//	match the size of the main move list
	m_aPlayouts.reserve(m_aMoveList.size());
	//	now reset the data stored in them ...
	for (int i = 0; i < m_aMoveList.size(); ++i)
	{
		m_aMoveScores.push_back(0.0f);
		m_aPlayouts.push_back(0);
	}

	//	the thread vector should be empty at this point - having had all threads joined before the end of the previous turn
	//	OK ... so the threading isn't being nice to me ...  time to drop back to not threaded ...
	/*
	for (int i = 0; i < m_iThreadsForThisTurn; ++i)
	{
		//	kick off the Conduct Playouts threads
		//m_AIThreads.push_back(std::thread(&ConductPlayouts, i, m_iThreadsForThisTurn, (int)m_aMoveList.size()));
		m_AIThreads.push_back(std::thread(&Player::ConductPlayouts, this, i));
	}
	*/
	//	we have the data needed to start processing the possible moves, so start the playouts
	ConductPlayouts();
}

void	Player::ConductPlayouts()
//void	Player::ConductPlayouts(int a_iThreadNum, int a_iTotalThreads, int a_iNumMovesInList)
{
	//									2					3					10
	//	this is spun off in its own thread
	//	worker thread to score the current section of the move list
	//	This way the thread will only work on its section of the move list, generating scores for the moves by conducting random playthroughs of each available move until the m_bAITimesUp flag is true, signalling there is no more time available.
//	int	iSectionLength = a_iNumMovesInList / a_iTotalThreads;	//	10 / 3 = 3
//	int	iMinBound = a_iThreadNum * iSectionLength;				//	2 * 3 = 6
	int	iSectionLength = 1;	//	10 / 3 = 3
	int	iMinBound = 0;				//	2 * 3 = 6
//	int oMaxBound = (a_iThreadNum + 1) * iSectionLength;		//	3 * 3 = 9
	int oMaxBound = m_aMoveList.size();
//	if (a_iThreadNum == (a_iTotalThreads - 1))
//	{
//		//	if this is the last thread, then make sure it checks all the way to the last move
//		oMaxBound = a_iNumMovesInList;
//	}
	//std::cout << "ConductPlayouts: Player: " << m_iPlayerNumber << "  Thread #:" << a_iThreadNum << '\n';
		
	//	ok back to single thread mode ... this function will need to manage its own time then ...
	float	fNewTime = (float)glfwGetTime();
	float	fPrevTime = fNewTime;
	float	dT = fNewTime - fPrevTime;	//	that'll be zero to start with ...
	float	fElapsedTime = 0.0f;
	Move	oTestMove;
	GameState	oCurrentGameState = m_pGame->m_oGameState;
	GameState	oTestState;
	//oTestState.m_iCurrentPlayer = oCurrentGameState.m_iCurrentPlayer;
	
	if (oCurrentGameState.m_iCurrentPlayer == 1)
	{
		oTestState.m_iCurrentPlayer = 2;
	}
	else
	{
		oTestState.m_iCurrentPlayer = 1;
	}
	
	//int	iCurrentMoveIndex = iMinBound;
	while (!m_bAITimesUp)
	{
		//	score playouts of each move in the range and add the score to the scores vector
		for (int i = iMinBound; i < oMaxBound; ++i)
		{
			//	get the move to test
			oTestMove = m_aMoveList[i];
			oTestState.m_Kings = oCurrentGameState.m_Kings;
			//	generate the new GameState for that move
			if (oCurrentGameState.m_iCurrentPlayer == 1)
			{
				//	Generate the new state after Player 1 has made the test move
				//	first check if a King has moved
				if ((oCurrentGameState.m_Kings & oTestMove.StartPos) > 0)
				{
					//	A King has been moved
					oTestState.m_Kings = (oCurrentGameState.m_Kings & ~oTestMove.StartPos) | oTestMove.EndPos;
				}
				oTestState.m_P1Pieces = (oCurrentGameState.m_P1Pieces & ~oTestMove.StartPos) | oTestMove.EndPos;
				oTestState.m_P2Pieces = oCurrentGameState.m_P2Pieces;
			}
			else
			{
				//	Generate the new state after Player 2 has made the test move
				//	first check if a King has moved
				if ((oCurrentGameState.m_Kings & oTestMove.StartPos) > 0)
				{
					//	A King has been moved
					oTestState.m_Kings = (oCurrentGameState.m_Kings & ~oTestMove.StartPos) | oTestMove.EndPos;
				}
				oTestState.m_P2Pieces = (oCurrentGameState.m_P2Pieces & ~oTestMove.StartPos) | oTestMove.EndPos;
				oTestState.m_P1Pieces = oCurrentGameState.m_P1Pieces;
			}
			if (oTestMove.bJumper)
			{
				if (oCurrentGameState.m_iCurrentPlayer == 1)
				{
					oTestState.m_iCurrentPlayer = 1;
				}
				else
				{
					oTestState.m_iCurrentPlayer = 2;
				}
				//	then we also need to remove the captured piece ...
				int	iCapturedX = (GetBoardXCoord(oTestMove.StartPos) + GetBoardXCoord(oTestMove.EndPos)) / 2;
				int	iCapturedY = (GetBoardYCoord(oTestMove.StartPos) + GetBoardYCoord(oTestMove.EndPos)) / 2;
				Bitboard	bbNotCaptured = ~GenerateBitMaskFromCoords(iCapturedX, iCapturedY);
				oTestState.m_Kings &= bbNotCaptured;
				oTestState.m_P1Pieces &= bbNotCaptured;
				oTestState.m_P2Pieces &= bbNotCaptured;
			}
			//	do some playouts in blocks and accumulate the scores
			int	iPlayouts = 20;
			for (int p = 0; p < iPlayouts; ++p)
			{
				m_aMoveScores[i] += ScorePlayout(m_iLookAheadThisTurn, oTestState);
			}
			m_aPlayouts[i] += iPlayouts;
		}

		fPrevTime = fNewTime;
		fNewTime = (float)glfwGetTime();
		dT = fNewTime - fPrevTime;	//	get the new dT
		fElapsedTime += dT;
		if (fElapsedTime > m_fMaxTimePerAIMove)
		{
			//	time's up, so flag an exit of the loop
			m_bAITimesUp = true;
		}
		//	just while evaluating the score testing and move selection ...
		//m_bAITimesUp = true;
	}
	//	reset the glfw time so there is not a massive dT when the next update happens ...
	glfwSetTime(0.0f);
	
	//	this would also need to be removed if I get back to threading
	m_bAIDecidingMove = false;
	//std::cout << " Exiting ConductPlayouts: Player: " << m_iPlayerNumber << "  Thread #:" << a_iThreadNum << '\n';
}

float	Player::ScorePlayout(int a_iLookAheadCountDown, GameState a_oGameState)
{
	//	This function conducts a playout
	float	fScore = 0.0f;
	if (a_oGameState.m_iCurrentPlayer == 1)
	{
		//	then player 2 has just made a move, so evaluate the board from player 2's perspective
		//	Player 2's move may have won them the game, so check if player 1 still has pieces left
		if (a_oGameState.m_P1Pieces == 0)
		{
			//	Player 1 has no pieces left, therefore they have lost, so this playout ends here
			return fScore = 250.0f;	//	big score for a win
		}
		else
		{
			fScore = ScoreCurrentBoard(2, a_oGameState);
		}
	}
	else
	{
		//	then player 1 has just made a move, so evaluate the board from player 1's perspective
		//	Player 1's move may have won them the game, so check if player 2 still has pieces left
		if (a_oGameState.m_P2Pieces == 0)
		{
			//	Player 2 has no pieces left, therefore they have lost, so this playout ends here
			return fScore = 250.0f;	//	big score for a win
		}
		else
		{
			fScore = ScoreCurrentBoard(1, a_oGameState);
		}
	}
	if (a_iLookAheadCountDown > 0)
	{
		//	we still have further moves to play out ...
		//	so this is where a new set of available moves needs to be generated, a random one picked, the game state modified accordingly
		//	and then recursively call ScorePlayout for the new move, subtracting the resulting score from the current score
		GameState	oNewState;
		Move	oNewMove;
		bool	bJumpers = false;
		{
			//	new level of scope with the intent of minimising recursive memory usage by the move lists
			std::vector<Move>	aMoveList;
			Bitboard	bbAvailableMovers = GetCurrentAvailableMovers(a_oGameState, bJumpers);
			AIGenerateMovesFromAvailableMovers(a_oGameState, bbAvailableMovers, bJumpers, aMoveList);
			if (aMoveList.size() == 0)
			{
				//	there are no available moves so simply return the score
				return fScore;
			}
			//	now pick a random move and add it to the new game state
			oNewMove = aMoveList[rand() % aMoveList.size()];
		}
		if (a_oGameState.m_iCurrentPlayer == 1)
		{
			//	player 1 is making the move
			//	first check if a King has moved
			if ((a_oGameState.m_Kings & oNewMove.StartPos) > 0)
			{
				//	A King has been moved
				oNewState.m_Kings = (a_oGameState.m_Kings & ~oNewMove.StartPos) | oNewMove.EndPos;
			}
			oNewState.m_P1Pieces = (a_oGameState.m_P1Pieces & ~oNewMove.StartPos) | oNewMove.EndPos;
			oNewState.m_P2Pieces = a_oGameState.m_P2Pieces;

			if (bJumpers)
			{
				oNewState.m_iCurrentPlayer = 1;
			}
			else
			{
				oNewState.m_iCurrentPlayer = 2;
			}
		}
		else
		{
			//	player 2 is making the move
			//	first check if a King has moved
			if ((a_oGameState.m_Kings & oNewMove.StartPos) > 0)
			{
				//	A King has been moved
				oNewState.m_Kings = (a_oGameState.m_Kings & ~oNewMove.StartPos) | oNewMove.EndPos;
			}
			oNewState.m_P2Pieces = (a_oGameState.m_P2Pieces & ~oNewMove.StartPos) | oNewMove.EndPos;
			oNewState.m_P1Pieces = a_oGameState.m_P1Pieces;

			if (bJumpers)
			{
				oNewState.m_iCurrentPlayer = 2;
			}
			else
			{
				oNewState.m_iCurrentPlayer = 1;
			}
		}
		if (bJumpers)
		{
			//	then we also need to remove the captured piece ...
			int	iCapturedX = (GetBoardXCoord(oNewMove.StartPos) + GetBoardXCoord(oNewMove.EndPos)) / 2;
			int	iCapturedY = (GetBoardYCoord(oNewMove.StartPos) + GetBoardYCoord(oNewMove.EndPos)) / 2;
			Bitboard	bbNotCaptured = ~GenerateBitMaskFromCoords(iCapturedX, iCapturedY);
			oNewState.m_Kings &= bbNotCaptured;
			oNewState.m_P1Pieces &= bbNotCaptured;
			oNewState.m_P2Pieces &= bbNotCaptured;
			//	it's still our turn so add the returning playout score
			fScore += ScorePlayout(a_iLookAheadCountDown - 1, oNewState);
		}
		else
		{
			//	it's the opposition's turn, so subtract the returning playout score
			fScore -= ScorePlayout(a_iLookAheadCountDown - 1, oNewState);
		}
	}
	return fScore;
}

void	Player::CheckScores()
{
	//	This function simply runs through the list of move scores and decides which has the best score per playout, then sets that as the selected move.
	float	fBestScorePerPlayout = -FLT_MAX;	//	worst possible score ...
	float	fTestScorePerPlayout;
	int		iBestScoreIndex = 0;
	for (int i = 0; i < m_aMoveScores.size(); ++i)
	{
		fTestScorePerPlayout = m_aMoveScores[i] / (float)m_aPlayouts[i];
		if (fTestScorePerPlayout > fBestScorePerPlayout)
		{
			fBestScorePerPlayout = fTestScorePerPlayout;
			iBestScoreIndex = i;
		}
	}
	if (iBestScoreIndex >= m_aMoveList.size())
	{
		//	then something went wrong somewhere, since the vectors should be the same size ...
		//	so just pick a random move ...
		iBestScoreIndex = rand() % m_aMoveList.size();
		//	hopefully we will never see the following debug print!
		std::cout << "Something went wrong with the scoring, so I'm picking a random move from the available list!!\n";
	}
	//	ok so now we've got a valid index to the move array, set that as the move and we're done!
	m_oSelectedMove = m_aMoveList[iBestScoreIndex];
}

void	Player::CloseThreads()
{
	//	closes any opened threads.
	if (m_AIThreads.size() > 0)
	{
		m_bAITimesUp = true;
		for (int i = m_AIThreads.size() - 1; i >= 0; --i)
		{
			//	close the ConductPlayouts threads
			m_AIThreads[i].join();	//	wait for the thread to close
			m_AIThreads.erase(m_AIThreads.begin() + i);	//	then delete the thread object
		}
		m_AIThreads.clear();	//	clean up the vector of anything remaining
	}
}

void	Player::CheckMoveDecisionStatus()
{
	if (m_fCurrentMoveTimer >= m_fMaxTimePerAIMove)
	{
		m_bAITimesUp = true;	//	signal the threads that they need to end now and return their current scores!
		//	This is where the thread join() call should be made, followed by the final decision.
		CloseThreads();

		//	now go through the scores and pick the best move ...
		//	just for initial testing
		m_oSelectedMove.StartPos = 0x00000200;
		m_oSelectedMove.EndPos = 0x00002000;
		if (ValidMove(m_oSelectedMove, m_pGame->m_oGameState))
		{
			m_fCurrentMoveTimer = 0.0f;
			m_bAIDecidingMove = false;
		}
		else
		{
			//	Houston, we have a problem with move generation!
		}
	}

	//	this function also needs to make sure the move decided is a valid move before setting the move decided flag to true

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

void	Player::AIGenerateMovesFromAvailableMovers(GameState a_oGameState, Bitboard a_bbMovers, bool a_bJumpers, std::vector<Move> &a_aMoveList)
{
	//	This function generates a list of available moves from the GameState and the bitboard of available moving pieces passed in.
	//	Also passed in is a flag indicating if the movers passed in are jumpers or not.  This simplifies the move generation requirements.
	//	The list is stored in the a_aMoveList vector (passed in by reference), replacing any moves previously stored in it.
	a_aMoveList.clear();
	Move	oTempMove;
	for (unsigned int i = 0; i < 32; ++i)
	{
		if ((a_bbMovers & abbSquareMasks[i]) > 0)
		{
			//	we have a moving piece, so generate any possible moves for that piece and store them in the move list vector
			//oTempMove.StartPos = abbSquareMasks[i];
			GenerateMovesForMover(i, a_oGameState, a_aMoveList, a_bJumpers);
		}
	}
}
