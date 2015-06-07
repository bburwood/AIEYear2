#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "Bitboard.h"

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
	float	ScoreCurrentBoard(int a_iPlayer, Bitboard a_bbP1Pieces, Bitboard a_bbP1Kings, Bitboard a_bbP2Pieces, Bitboard a_bbP2Kings);
	void	MakeMoveDecision();
	void	MouseClickedOnBoardAt(int iBoardX, int iBoardY);
	void	SetCheckersPointer(Checkers* a_pCheckers);
	void	GetSelectedMoveDetails(bool& a_bPieceSelected, int& a_iXCoord, int& a_iZCoord);
	void	GenerateMovesFromAvailableMovers(Bitboard a_bbMovers, bool a_bJumpers);
	//bool	ValidMove(Move a_oTestMove, GameState a_oCurrentState);


	PlayerType	m_ePlayerType;	//	public so that it can be easily accessed and changed from AntTweakBar

private:
	Move	m_oSelectedMove;
	int		m_iPlayerNumber;
	float	m_fCurrentMoveTimer;
	float	m_fMaxTimePerAIMove;
	int		m_iMouseX;
	int		m_iMouseY;
	Game*	m_pGame;	//	so we can call the game to return moves once decided
	Checkers*	m_pProgram;	//	use this ONLY for firing off the particle emitters ...
	bool	m_bNotMyTurn;
	bool	m_bPieceSelected;
	bool	m_bMoveInProgress;
	bool	m_bMoveCompleted;
};

#endif	//	_PLAYER_H_
