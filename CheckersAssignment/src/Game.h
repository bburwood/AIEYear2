#ifndef _GAME_H_
#define _GAME_H_

#include "Bitboard.h"
#include "Player.h"
#include <vector>

class	Checkers;

class Game
{
public:
	Game();
	~Game();
	void	update(float dT);
	void	FindValidMoves();
	void	MakeMove(Move a_oMove);
	void	RemoveCapturedPiece(Bitboard a_bbCaptured);
	void	DecideMove();
	void	ResetGame(int a_iFirstMover);
	void	MouseClickedOnBoardAt(int iBoardX, int iBoardY);
	void	EndTurn();
	void	SetPlayerCheckersPointer(Checkers* a_pCheckers);
	void	GetSelectedMoveDetails(bool& a_bPieceSelected, int& a_iXCoord, int& a_iZCoord);
	void	CheckForGameOver();
	void	SetCheckersPointer(Checkers* a_pCheckers);

	GameState	m_oGameState;	//	current state of the game
	std::vector<Move>	m_aMoveList;
	int		m_iNoCaptureTurnsLimit;
	int		m_iTurnsSinceLastCapture;
	float	m_fMoveSpeed;
	Player	m_P1;	//	public for AntTweakBar player type access ...
	Player	m_P2;

private:
	Move	m_SelectedMove;
	Checkers*	m_pProgram;	//	use this ONLY for firing off the particle emitters ...
	vec3	m_vMovingPosition;
	vec3	m_vMoveDirection;
	vec3	m_vStartPosition;
	vec3	m_vEndPosition;
	int		m_iCapturePosX;
	int		m_iCapturePosY;
	int		m_iPlayerMoving;
	Bitboard	m_bbCapturedPiece;
	Bitboard	m_bbFinalMovePosition;
	bool	m_bPieceMoving;	//	flags when a move is in progress, for animation purposes.
	bool	m_bMoveStarted;	//	set to true once a player has selected the first move for this turn.
	//	This is so that the game will still force any further captures for the current turn.
	bool	m_bMoveEnded;	//	When m_bMoveStarted is true, this is set to true once the current player can no longer capture any further pieces, ending their turn.
	bool	m_bGameOver;
	bool	m_bJumpMove;	//	flags if the moving piece is a jump move
	//	adding a full game undo would also be fairly easy ... simply store the game state each time someone moves, into a vector of gamestates
	//	when a move is undone just delete the last element and restore the game state of the new last element
	//	The main tricky part is to interrupt the AI making decisions when an undo is being done ...
	//	At only 4 ints/unsigned ints per move, storage would be trivial. --> 1024 moves in 4kB.
};

#endif	//	_GAME_H_
