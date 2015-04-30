#pragma once

#include "AIBase.h"

// classic connect four style game with 7 columns and 6 rows
class ConnectFour : public Game {
public:

	enum {
		COLUMNS = 7,
		ROWS = 6,
	};

	ConnectFour() :
		m_bluePieces(0),
		m_redPieces(0) {
		for (int& i : m_piecesPerColumn)
			i = 0;
	}
	virtual ~ConnectFour() {}

	// returns wether someone is winning or not
	virtual State	getCurrentGameState() const;

	// get a list of valid actions for current player
	virtual void	getValidActions(std::vector<int>& actions) const {
		actions.clear();
		for (int i = 0; i < COLUMNS; i++)
			if (isActionValid(i))
				actions.push_back(i);
	}

	virtual bool	isActionValid(int action) const {
		return action >= 0 && action < COLUMNS && m_piecesPerColumn[action] < ROWS;
	}

	// performs an action for the current player, and switches current player
	virtual void	performAction(int action);

	// draw the game
	virtual void	draw() const;

	// clones the current game state
	virtual Game*	clone() const {
		ConnectFour* game = new ConnectFour();
		game->m_currentPlayer = this->m_currentPlayer;
		game->m_currentOpponent = this->m_currentOpponent;
		game->m_bluePieces = this->m_bluePieces;
		game->m_redPieces = this->m_redPieces;
		for (int i = 0; i < COLUMNS; i++)
			game->m_piecesPerColumn[i] = this->m_piecesPerColumn[i];
		return game;
	}

private:

	State	getPieceAt(int r, int c) const;

	long long		m_bluePieces;
	long long		m_redPieces;

	int				m_piecesPerColumn[COLUMNS];

};