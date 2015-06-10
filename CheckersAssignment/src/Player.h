#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "Bitboard.h"
#include <thread>
#include <vector>

class	Game;
class	Checkers;

enum PlayerType
{
	PLAYER_HUMAN,
	PLAYER_MCTS_AI,
};

class Player
{
public:
	Player();
	~Player();
	//	General functions
	void	InitPlayer(int a_iPlayerNumber, PlayerType a_ePlayerType, float a_fMaxTimePerAIMove, Game* a_pGame);
	void	update(float dT, GameState a_oCurrentGameState);
	//	AI functions
	float	ScoreCurrentBoard(int a_iPlayer, GameState a_oGameState);
	void	BeginMoveDecision();
	void	CheckMoveDecisionStatus();
	void	MouseClickedOnBoardAt(int iBoardX, int iBoardY);
	void	SetCheckersPointer(Checkers* a_pCheckers);
	void	GetSelectedMoveDetails(bool& a_bPieceSelected, int& a_iXCoord, int& a_iZCoord);
	void	GenerateMovesFromAvailableMovers(Bitboard a_bbMovers, bool a_bJumpers);
	void	AIGenerateMovesFromAvailableMovers(GameState a_oGameState, Bitboard a_bbMovers, bool a_bJumpers, std::vector<Move> &a_aMoveList);
//	void	ConductPlayouts(int a_iThreadNum, int a_iTotalThreads, int a_iNumMovesInList);	//	worker thread to score its move list segment
	void	ConductPlayouts();	//	worker thread to score its move list segment ... no so much a thread any more ...
	float	ScorePlayout(int a_iLookAheadCountDown, int a_iLastMover, GameState a_oGameState);
	void	CheckScores();
	void	CloseThreads();
	//bool	ValidMove(Move a_oTestMove, GameState a_oCurrentState);


	PlayerType	m_ePlayerType;	//	public so that it can be easily accessed and changed from AntTweakBar
	int		m_iAIThreads;
	int		m_iAILookAheadMoves;
	float	m_fMaxTimePerAIMove;

private:
	std::vector<std::thread>	m_AIThreads;
	std::vector<Move>	m_aMoveList;	//	this is the initial move list for the current turn of the game, used by the AI
	std::vector<float>	m_aMoveScores;	//	this should store the cumulative score obtained for the corresponding move in the move list
	std::vector<int>	m_aPlayouts;	//	this stores the number of playouts that this move has been involved in
	Move	m_oSelectedMove;
	int		m_iPlayerNumber;
	int		m_iThreadsForThisTurn;	//	to allow for the use changing the number of threads on the fly - the change will kick in on the following AI turn.
	int		m_iLookAheadThisTurn;
	Bitboard	m_bbAICurrentAvailableMovers;
	float	m_fCurrentMoveTimer;
	int		m_iMouseX;
	int		m_iMouseY;
	Game*	m_pGame;	//	so we can call the game to return moves once decided
	Checkers*	m_pProgram;	//	use this ONLY for firing off the particle emitters ...
	bool	m_bNotMyTurn;
	bool	m_bAIDecidingMove;
	bool	m_bAITimesUp;
	bool	m_bPieceSelected;
	bool	m_bMoveInProgress;
	bool	m_bMoveCompleted;
};

#endif	//	_PLAYER_H_
