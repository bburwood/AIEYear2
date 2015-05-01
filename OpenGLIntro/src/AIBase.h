#pragma once

#include <vector>
#include <stdlib.h>

// base class to define a zero-sum game with two opponents
// actions are defined as int's
class Game {
public:

	enum State {
		UNKNOWN,
		PLAYER_ONE,
		PLAYER_TWO,
		DRAW,
	};

	Game() :
		m_currentPlayer(PLAYER_ONE),
		m_currentOpponent(PLAYER_TWO) {

	}
	virtual ~Game() {}

	// returns whether someone is winning or not
	virtual State	getCurrentGameState() const = 0;

	// these switch whenever an action is made
	State			getCurrentPlayer() const	{ return m_currentPlayer;	}
	State			getCurrentOpponent() const	{ return m_currentOpponent; }

	// get a list of valid actions for current player
	virtual void	getValidActions(std::vector<int>& actions) const = 0;
	virtual bool	isActionValid(int action) const = 0;

	// performs an action for the current player, and switches current player
	virtual void	performAction(int action) = 0;

	// draw the game
	virtual void	draw() const = 0;

	// clones the current game state
	virtual Game*	clone() const = 0;

//protected:

	State	m_currentPlayer;
	State	m_currentOpponent;
};

// base class for an A.I. opponent
class AI {
public:

	AI() {}
	virtual ~AI() {}

	// makes a decision for the current game
	virtual int	makeDecision(const Game& game) = 0;
};

// an A.I. opponent that chooses purely random actions to perform
class RandomAI : public AI {
public:

	RandomAI() {}
	virtual ~RandomAI() {}

	// randomly choose an action to perform
	virtual int	makeDecision(const Game& game) {
		std::vector<int> actions;
		game.getValidActions(actions);
		if (actions.size() > 0)
			return actions[rand() % actions.size()];
		else
			return -1;
	}
};

// My A.I. opponent: MCTS (Monte Carlo Tree Search)
class MCTS : public AI {
public:

	MCTS(int playouts) :
		m_iPlayouts(playouts)
	{}
	virtual ~MCTS() {}

	virtual int	makeDecision(const Game& game)
	{
		std::vector<int> actions;
		std::vector<int> bestActions;
		game.getValidActions(actions);
		if (actions.size() > 0)
		{
			//	then we have valid moves to consider
			if (actions.size() == 1)
			{
				//	if there is only 1 action we can make, then simply return it now
				return actions[0];
			}
			int uiChosenAction = -1;

			//	first check if we have a move that will immediately win
			for (unsigned int g = 0; g < actions.size(); ++g)
			{
				Game* cloneGame = game.clone();
				Game::State currPlayer = cloneGame->getCurrentPlayer();
				Game::State otherPlayer = cloneGame->getCurrentOpponent();

				cloneGame->performAction(actions[g]);

				if (cloneGame->getCurrentGameState() == currPlayer)
				{
					//	if we have found a winning move then return it immediately
					delete cloneGame;
					return actions[g];
				}
				delete cloneGame;
			}
			//	then check if there are any moves where the opposition will win next turn - and go there instead!
			for (unsigned int g = 0; g < actions.size(); ++g)
			{
				Game* cloneGame = game.clone();
				Game::State currPlayer = cloneGame->getCurrentPlayer();
				Game::State otherPlayer = cloneGame->getCurrentOpponent();
				//	now swap the players in the clone game around to see if the opposition has a winning move next
				cloneGame->m_currentOpponent = currPlayer;
				cloneGame->m_currentPlayer = otherPlayer;

				cloneGame->performAction(actions[g]);

				if (cloneGame->getCurrentGameState() == otherPlayer)
				{
					//	if we have found an opposition winning move then return it immediately
					delete cloneGame;
					return actions[g];
				}
				delete cloneGame;
			}

			//	ok now we have checked for winning moves or blocking opposition winning moves, now let's do
			//	something *non*-random for other moves
			//	run through each eaction and do m_iPlayouts random play throughs for each action to work out a high scoring next move
			int iBestAction = 0;
			float fBestScore = -9999999999.0f;	//	large negative to start with
			for (unsigned int g = 0; g < actions.size(); ++g)
			{
				int iValue = 0;
				for (int p = 0; p < m_iPlayouts; ++p)
				{
					Game* cloneGame = game.clone();
					cloneGame->performAction(actions[g]);
					while (cloneGame->getCurrentGameState() == Game::UNKNOWN)
					{
						//	while we have no winner, or a draw, make another move
						std::vector<int> tempActions;
						cloneGame->getValidActions(tempActions);
						cloneGame->performAction(tempActions[rand() % tempActions.size()]);
					}
					//	the game should now be ended, so work out who won (or a draw) and score accordingly
					if (cloneGame->getCurrentGameState() == game.getCurrentPlayer())
					{
						//	we won, yay!
						++iValue;
					}
					else if (cloneGame->getCurrentGameState() == game.getCurrentOpponent())
					{
						//	the other guy won ... :-(
						--iValue;
					}
					//	no score change for a draw
					delete cloneGame;
				}
				float fTempScore = (float)iValue / (float)m_iPlayouts;
				if (fTempScore > fBestScore)
				{
					//	we have a new winning move
					iBestAction = g;
					fBestScore = fTempScore;
					//	now reset the best actions vector
				}
				else if (fTempScore == fBestScore)
				{
					//	add it to the list of highest scores ...
				}
			}
			return actions[iBestAction];
		}
		else
			return -1;
	}
private:
	//	the number of times it will simulate a game
	int	m_iPlayouts;
};
