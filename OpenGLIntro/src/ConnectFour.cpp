#include "ConnectFour.h"
#include "Gizmos.h"
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

Game::State ConnectFour::getCurrentGameState() const {
	// check draw
	bool draw = true;

	for (int col = 0; col < COLUMNS; col++)	{
		if (isActionValid(col))	{
			draw = false;
			break;
		}
	}

	if (draw)
		return Game::DRAW;

	// test player two
	long long y = m_bluePieces & (m_bluePieces >> 6);
	if ((y & (y >> 2 * 6)) > 0) // check \ diagonal
		return Game::PLAYER_TWO;
	y = m_bluePieces & (m_bluePieces >> 7);
	if ((y & (y >> 2 * 7)) > 0) // check horizontal -
		return Game::PLAYER_TWO;
	y = m_bluePieces & (m_bluePieces >> 8);
	if ((y & (y >> 2 * 8)) > 0) // check / diagonal
		return Game::PLAYER_TWO;
	y = m_bluePieces & (m_bluePieces >> 1);
	if ((y & (y >> 2)) > 0)     // check vertical |
		return Game::PLAYER_TWO;

	// test player one
	long long yy = m_redPieces & (m_redPieces >> 6);
	if ((yy & (yy >> 2 * 6)) > 0) // check \ diagonal
		return Game::PLAYER_ONE;
	yy = m_redPieces & (m_redPieces >> 7);
	if ((yy & (yy >> 2 * 7)) > 0) // check horizontal -
		return Game::PLAYER_ONE;
	yy = m_redPieces & (m_redPieces >> 8);
	if ((yy & (yy >> 2 * 8)) > 0) // check / diagonal
		return Game::PLAYER_ONE;
	yy = m_redPieces & (m_redPieces >> 1);
	if ((yy & (yy >> 2)) > 0)     // check vertical |
		return Game::PLAYER_ONE;

	// no winner
	return Game::UNKNOWN;
}

void ConnectFour::performAction(int action) {
	// get the location on the board
	long long pos = ((long long)1 << (m_piecesPerColumn[action] + 7 * action));

	// set the piece based on current player
	if (m_currentPlayer == Game::PLAYER_ONE)
		m_redPieces ^= pos;
	else
		m_bluePieces ^= pos;

	// increase the pieces in the column
	m_piecesPerColumn[action]++;

	// switch players around
	m_currentPlayer = (m_currentPlayer == Game::PLAYER_TWO) ? Game::PLAYER_ONE : Game::PLAYER_TWO;
	m_currentOpponent = (m_currentOpponent == Game::PLAYER_TWO) ? Game::PLAYER_ONE : Game::PLAYER_TWO;
}

Game::State ConnectFour::getPieceAt(int r, int c) const {
	long long number = ((long long)1 << (r + 7 * c));

	bool test = (m_bluePieces & number) > 0;
	if (test)
		return Game::PLAYER_TWO;
	test = (m_redPieces & number) > 0;
	if (test)
		return Game::PLAYER_ONE;
	return Game::UNKNOWN;
}

void ConnectFour::draw() const {
	using glm::vec3;
	using glm::vec4;

	// draw grid lines
	for (int i = 0; i <= COLUMNS; i++) {
		Gizmos::addLine(vec3(ROWS, 0, -COLUMNS + i * 2), vec3(-ROWS, 0, -COLUMNS + i * 2), vec4(0, 0, 0, 1));
	}
	for (int i = 0; i <= ROWS; i++) {
		Gizmos::addLine(vec3(-ROWS + i * 2, 0, -COLUMNS), vec3(-ROWS + i * 2, 0, COLUMNS), vec4(0, 0, 0, 1));
	}

	// draw pieces
	for (int row = 0; row < ROWS; row++) {
		for (int col = 0; col < COLUMNS; col++) {
			vec3 pos(-ROWS + 1 + row * 2, 0, -COLUMNS + 1 + col * 2);
			if (getPieceAt(row, col) == Game::PLAYER_ONE) {
				Gizmos::addDisk(pos, 0.75f, 6, vec4(0, 0, 1, 1));
			}
			else if (getPieceAt(row, col) == Game::PLAYER_TWO) {
				Gizmos::addDisk(pos, 0.75f, 6, vec4(1, 0, 0, 1));
			}
		}
	}
}