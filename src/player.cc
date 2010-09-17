/*
 * Copyright (c) 2010 Jason Lynch <jason@calindora.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <iomanip>
#include <iostream>
#include <limits>
#include <list>
#include <memory>
#include <set>

#include "tripletriad.hh"
#include "player.hh"

// Define macros for finding the minimum or maximum of two inputs.
#define MIN(X, Y)  ((X) < (Y) ? (X) : (Y))
#define MAX(X, Y)  ((X) > (Y) ? (X) : (Y))

Player::Player(GameBoard *gameBoard, Piece myPiece, Piece opponentPiece)
{
	this->_realGameBoard = gameBoard;
	this->_gameBoard = NULL;
	this->_myPiece = myPiece;
	this->_opponentPiece = opponentPiece;
	this->_abortedPly = false;
	this->_totalPly = 0;
	this->_searchCount = 0;
	
	this->_pieceWeight = 0;
	this->_mobilityWeight = 40;
	this->_pMobilityWeight = 30;
	this->_cornerWeight = 100;
	this->_winWeight = 450;
}

Player::~Player()
{
	std::cout << "Ran " << this->_searchCount << " searches, with an average ply reached of " << ((double)this->_totalPly / this->_searchCount) << std::endl;
}

void Player::move(unsigned int moveTime)
{
	// Make a copy of the game board.
	if (this->_gameBoard != NULL) delete this->_gameBoard;
	this->_gameBoard = new GameBoard(*(this->_realGameBoard));

	int currentPly = 1;
	std::shared_ptr<Move> moveToMake;
	this->_moveTime = SDL_GetTicks() + moveTime;
	this->_abortedPly = false;

	bool continueSearch = true;
	int bestPly = 0;
	int firstGuess = 0;

	//this->_transpositionTable.reset();

	while (continueSearch == true)
	{
		// Declare necessary variables.
		int bestValue = std::numeric_limits<int>::min();
		std::shared_ptr<Move> bestMove;
		this->_positions = 0;
		this->_currentPly = 0;
		this->_maxPly = 0;

		// Get the list of moves.
		std::set<std::shared_ptr<Move>> moveList = this->_gameBoard->get_valid_moves();

		// Iterate through the list, finding the best one.
		std::set<std::shared_ptr<Move>>::iterator iter = moveList.begin();
		while (iter != moveList.end() && continueSearch == true)
		{
			// Make the move on the board.
			//std::cout << "move(): Trying to move " << (*(*iter)) << std::endl;
			this->_gameBoard->move(*iter);
			this->_positions++;
			int value = this->minimaxSearch(currentPly - 1);
			//int value = this->alphaBetaSearch(currentPly - 1, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
			//int value = this->alphaBetaMemorySearch(currentPly - 1, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
			//int value = this->MTDSearch(currentPly - 1, firstGuess);
			this->_gameBoard->unmove();

			if (this->_abortedPly == true)
			{
				break;
			}

			if (value > bestValue)
			{
				bestValue = value;
				bestMove = (*iter);
			}

			iter++;
		}
		
		firstGuess = bestValue;

		if (this->_abortedPly == false && this->_maxPly >= currentPly)
		{
			bestPly = currentPly;
			moveToMake = bestMove;

			// Print out a status message.
			std::cout << std::left;
			std::cout << std::setw(12) << "Search Ply:" << std::setw(4) << currentPly;
			std::cout << std::setw(11) << "Positions:" << std::setw(12) << this->_positions;
			std::cout << std::setw(6) << "Move:" << std::setw(30) << (*bestMove);
			std::cout << std::setw(10) << " Utility:" << std::setw(10) << bestValue << std::endl;
		}
		else
		{
			continueSearch = false;
		}

		currentPly++;
	}

	this->_totalPly += bestPly;
	this->_searchCount++;

	std::cout << moveToMake << std::endl;
	if (moveToMake == NULL || this->_realGameBoard->is_valid_move(moveToMake) == false)
	{
		if (moveToMake == NULL)
		{
			std::cout << "ERROR: moveToMake was null. Making first available valid move." << std::endl;
		}
		else
		{
			std::cout << "ERROR: moveToMake was an invalid move. Replacing with first available valid move." << std::endl;
		}

		std::set<std::shared_ptr<Move>> moveList = this->_realGameBoard->get_valid_moves();
		moveToMake = *(moveList.begin());
	}
	this->_realGameBoard->move(moveToMake);
	std::cout << "-------------------------" << std::endl;
}

void Player::setWeights(int pieceWeight, int mobilityWeight, int pMobilityWeight, int cornerWeight, int winWeight)
{
	this->_pieceWeight = pieceWeight;
	this->_mobilityWeight = mobilityWeight;
	this->_pMobilityWeight = pMobilityWeight;
	this->_cornerWeight = cornerWeight;
	this->_winWeight = winWeight;
}

int Player::minimaxSearch(int ply)
{
	// Check for time violation.
	if ((unsigned int)SDL_GetTicks() > this->_moveTime)
	{
		this->_abortedPly = true;
		return 0;
	}

	if (this->_positions % 50000 == 0)
	{
		if (TripleTriad::get_instance()->checkEvent(false) == true)
		{
			this->_abortedPly = true;
			return 0;
		}
	}

	// Set the maximum ply.
	this->_currentPly++;
	if (this->_currentPly > this->_maxPly)
	{
		this->_maxPly = this->_currentPly;
	}
	// Get the list of available moves.
	std::set<std::shared_ptr<Move>> moveList = this->_gameBoard->get_valid_moves();

	// Check for a terminal node.
	if (moveList.empty() || ply == 0)
	{
		this->_currentPly--;
		return this->evaluateBoard();
	}



	// Set the initial best value.
	int bestValue = (this->_gameBoard->get_current_piece() == this->_myPiece) ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();

	// Iterate through the moves, finding the best one.
	std::set<std::shared_ptr<Move>>::iterator iter = moveList.begin();
	while (iter != moveList.end())
	{
		// Make the move on the board.
		this->_gameBoard->move(*iter);
		this->_positions++;
		int value = this->minimaxSearch(ply - 1);
		this->_gameBoard->unmove();

		if (this->_gameBoard->get_current_piece() == this->_myPiece)
		{
			if (value > bestValue)
			{
				bestValue = value;
			}
		}
		else
		{
			if (value < bestValue)
			{
				bestValue = value;
			}
		}

		iter++;
	}

	this->_currentPly--;
	return bestValue;
}
/*
int Player::alphaBetaSearch(int ply, int alpha, int beta)
{
	// Check for time violation.
	if ((unsigned int)SDL_GetTicks() > this->_moveTime)
	{
		this->_abortedPly = true;
		return 0;
	}

	if (this->_positions % 50000 == 0)
	{
		if (this->_game->checkEvent(false) == true)
		{
			this->_abortedPly = true;
			return 0;
		}
	}

	// Set the maximum ply.
	this->_currentPly++;
	if (this->_currentPly > this->_maxPly)
	{
		this->_maxPly = this->_currentPly;
	}
	
	// Alpha-beta return value.
	int returnValue;

	// Check for a terminal node.
	if (this->_gameBoard->gameComplete() || ply == 0)
	{
		returnValue = this->evaluateBoard();
	}
	else
	{
		// Get the list of available moves.
		std::list<Move*> moveList;
		this->_gameBoard->get_valid_moves(moveList, this->_gameBoard->getCurrentPiece());
		
		if (this->_gameBoard->getCurrentPiece() == this->_myPiece)
		{
			returnValue = std::numeric_limits<int>::min();

			// We want to save the original alpha, so we'll use an extra variable.
			int subAlpha = alpha;

			// Iterate through the moves.
			std::list<Move*>::iterator iter = moveList.begin();
			while (iter != moveList.end() && returnValue < beta)
			{
				this->_gameBoard->move(*iter);
				this->_positions++;
				int value = this->alphaBetaSearch(ply - 1, subAlpha, beta);
				this->_gameBoard->unmove();

				if (value > returnValue) returnValue = value;
				if (returnValue > subAlpha) subAlpha = returnValue;

				iter++;
			}
		}
		else
		{
			returnValue = std::numeric_limits<int>::max();

			// We want to save the original beta, so we'll use an extra variable.
			int subBeta = beta;

			// Iterate through the moves.
			std::list<Move*>::iterator iter = moveList.begin();
			while (iter != moveList.end() && returnValue > alpha)
			{
				this->_gameBoard->move(*iter);
				this->_positions++;
				int value = this->alphaBetaSearch(ply - 1, alpha, subBeta);
				this->_gameBoard->unmove();
				
				if (value < returnValue) returnValue = value;
				if (returnValue < subBeta) subBeta = returnValue;

				iter++;
			}			
		}
	}

	this->_currentPly--;
	return returnValue;
}

int Player::alphaBetaMemorySearch(int ply, int alpha, int beta)
{
	// Check for time violation.
	if ((unsigned int)SDL_GetTicks() > this->_moveTime)
	{
		this->_abortedPly = true;
		return 0;
	}

	if (this->_positions % 5000 == 0)
	{
		this->_game->checkEvent(false);
	}

	// Set the maximum ply.
	this->_currentPly++;
	if (this->_currentPly > this->_maxPly)
	{
		this->_maxPly = this->_currentPly;
	}

//	std::cout << "Entering alphabeta(): " << this->_currentPly << std::endl;
	
	// Check the transposition table.
	unsigned long long boardKey = this->_gameBoard->getZobristKey();
	TranspositionTable::Entry *tableEntry = this->_transpositionTable.getEntry(boardKey);
	if (tableEntry == NULL)
	{
		tableEntry = this->_transpositionTable.newEntry(boardKey);
	}

	// Check against the transposition table entry.
	if (tableEntry->ply >= ply)
	{
		if (tableEntry->lowerBound >= beta)
		{
			this->_maxPly = MAX(this->_maxPly, MIN(this->_gameBoard->getEmptySquares(), tableEntry->ply));
//			std::cout << "Exiting alphabeta(): " << this->_currentPly << std::endl;
			this->_currentPly--;
			return tableEntry->lowerBound;
		}
		if (tableEntry->upperBound <= alpha)
		{
			this->_maxPly = MAX(this->_maxPly, MIN(this->_gameBoard->getEmptySquares(), tableEntry->ply));
//			std::cout << "Exiting alphabeta(): " << this->_currentPly << std::endl;
			this->_currentPly--;
			return tableEntry->upperBound;	
		}

		alpha = MAX(alpha, tableEntry->lowerBound);
		beta = MIN(beta, tableEntry->upperBound);
	}

	// Alpha-beta return value.
	int returnValue;

	// Check for a terminal node.
	if (this->_gameBoard->gameComplete() || ply == 0)
	{
		returnValue = this->evaluateBoard();
	}
	else
	{
		// Get the list of available moves.
		std::list<Move*> moveList;
		this->_gameBoard->get_valid_moves(moveList, this->_gameBoard->getCurrentPiece());
		if (tableEntry->bestMove != NULL)
		{
			moveList.remove(tableEntry->bestMove);
			//std::cout << "Pushing move: " << (*(tableEntry->bestMove)) << std::endl;
			moveList.push_front(tableEntry->bestMove);
		}
		
		if (this->_gameBoard->getCurrentPiece() == this->_myPiece)
		{
			returnValue = std::numeric_limits<int>::min();

			// We want to save the original alpha, so we'll use an extra variable.
			int subAlpha = alpha;

			// Iterate through the moves.
			std::list<Move*>::iterator iter = moveList.begin();
			while (iter != moveList.end() && returnValue < beta)
			{
//				std::cout << "alphabeta(): Trying to move me " << (*(*iter)) << std::endl;				
				this->_gameBoard->move(*iter);
				this->_positions++;
				int value = this->alphaBetaMemorySearch(ply - 1, subAlpha, beta);
				this->_gameBoard->unmove();

				if (value > returnValue)
				{
					returnValue = value;
					if (this->_abortedPly == false) tableEntry->bestMove = *iter;
				}
				if (returnValue > subAlpha) subAlpha = returnValue;

				iter++;
			}
		}
		else
		{
			returnValue = std::numeric_limits<int>::max();

			// We want to save the original beta, so we'll use an extra variable.
			int subBeta = beta;

			// Iterate through the moves.
			std::list<Move*>::iterator iter = moveList.begin();
			while (iter != moveList.end() && returnValue > alpha)
			{
//				std::cout << "alphabeta(): Trying to move opponent " << (*(*iter)) << std::endl;
				this->_gameBoard->move(*iter);
				this->_positions++;
				int value = this->alphaBetaMemorySearch(ply - 1, alpha, subBeta);
				this->_gameBoard->unmove();
				
				if (value < returnValue)
				{
					returnValue = value;
					if (this->_abortedPly == false) tableEntry->bestMove = *iter;
				}
				if (returnValue < subBeta) subBeta = returnValue;

				iter++;
			}			
		}
	}

	// Save new values to the transposition table.
	if (tableEntry->ply < ply)
	{
		tableEntry->lowerBound = std::numeric_limits<int>::min();
		tableEntry->upperBound = std::numeric_limits<int>::max();
	}

	if (tableEntry->ply <= ply)
	{
		if (returnValue <= alpha)
		{
			tableEntry->upperBound = returnValue;
		}
		else if (returnValue > alpha && returnValue < beta)
		{
			tableEntry->lowerBound = returnValue;
			tableEntry->upperBound = returnValue;
		}
		else if (returnValue >= beta)
		{
			tableEntry->lowerBound = returnValue;
		}
		tableEntry->ply = ply;
	}

//	std::cout << "Exiting alphabeta(): " << this->_currentPly << std::endl;
	
	this->_currentPly--;
	return returnValue;
}

int Player::MTDSearch(int ply, int firstGuess)
{
	int returnValue = firstGuess;
	int upperBound = std::numeric_limits<int>::max();
	int lowerBound = std::numeric_limits<int>::min();
	int beta;

	do
	{
		if (returnValue == lowerBound)
		{
			beta = returnValue + 1;
		}
		else
		{
			beta = returnValue;
		}

		returnValue = this->alphaBetaMemorySearch(ply, beta - 1, beta);

		if (returnValue < beta)
		{
			upperBound = returnValue;
		}
		else
		{
			lowerBound = returnValue;
		}
	}
	while (lowerBound < upperBound);

	return returnValue;
}
*/
int Player::evaluateBoard()
{
	int pieceScore = 0;
	
	// Determine the piece count score.
	int myPieces = this->_gameBoard->get_score(this->_myPiece);
	int opponentPieces = this->_gameBoard->get_score(this->_opponentPiece);
	pieceScore += myPieces;
	pieceScore -= opponentPieces;
	if (pieceScore % 2 == 1)
	{
		std::cout << "I have " << myPieces << " cards and you have " << opponentPieces << " cards." << std::endl;
//		this->_gameBoard->printBoard();
	}
	
	return pieceScore;
	
	/*
	// Declare the various scores.
	int mobilityScore  = 0;
	int pMobilityScore = 0;
	int pieceScore     = 0;
	int cornerScore    = 0;
	int winScore       = 0;

	// Retrieve the board itself.
	Square *board = this->_gameBoard->getBoard();

	// Determine mobility.
	std::list<Move*> moveList;
	this->_gameBoard->get_valid_moves(moveList, this->_myPiece);
	mobilityScore += (int)moveList.size();
	this->_gameBoard->get_valid_moves(moveList, this->_opponentPiece);
	mobilityScore -= (int)moveList.size();

	// Determine the piece count score.
	int myPieces = this->_gameBoard->getNumPieces(this->_myPiece);
	int opponentPieces = this->_gameBoard->getNumPieces(this->_opponentPiece);
	pieceScore += myPieces;
	pieceScore -= opponentPieces;

	// Determine the score for victory condition.
	if (this->_gameBoard->gameComplete())
	{
		if (pieceScore > 0)
		{
			winScore = 1;
		}
		else if (pieceScore < 0)
		{
			winScore = -1;
		}
	}

	// Check the four corners.
	if (board[ROWCOL_TO_SQUARE(0, 0)] == this->_myPiece) cornerScore++;
	if (board[ROWCOL_TO_SQUARE(0, 0)] == this->_opponentPiece) cornerScore--;
	if (board[ROWCOL_TO_SQUARE(0, 7)] == this->_myPiece) cornerScore++;
	if (board[ROWCOL_TO_SQUARE(0, 7)] == this->_opponentPiece) cornerScore--;
	if (board[ROWCOL_TO_SQUARE(7, 0)] == this->_myPiece) cornerScore++;
	if (board[ROWCOL_TO_SQUARE(7, 0)] == this->_opponentPiece) cornerScore--;
	if (board[ROWCOL_TO_SQUARE(7, 7)] == this->_myPiece) cornerScore++;
	if (board[ROWCOL_TO_SQUARE(7, 7)] == this->_opponentPiece) cornerScore--;

	// Calculate potential mobility. We could either count each empty square, or each piece next to an empty square.
	for (int row = 0; row < 8; row++)
	{
		for (int col = 0; col < 8; col++)
		{
			int square = ROWCOL_TO_SQUARE(row, col);
			Piece squarePiece = board[square];
			if (squarePiece != PIECE_NONE)
			{
				int modifier = (squarePiece == this->_myPiece) ? -1 : 1;
				if (board[square + GameBoard::N ] == PIECE_NONE) pMobilityScore += modifier;
				if (board[square + GameBoard::NE] == PIECE_NONE) pMobilityScore += modifier;
				if (board[square + GameBoard::E ] == PIECE_NONE) pMobilityScore += modifier;
				if (board[square + GameBoard::SE] == PIECE_NONE) pMobilityScore += modifier;
				if (board[square + GameBoard::S ] == PIECE_NONE) pMobilityScore += modifier;
				if (board[square + GameBoard::SW] == PIECE_NONE) pMobilityScore += modifier;
				if (board[square + GameBoard::W ] == PIECE_NONE) pMobilityScore += modifier;
				if (board[square + GameBoard::NW] == PIECE_NONE) pMobilityScore += modifier;
			}
		}
	}

	// Do some quick normalization.
	double pieceTerm = ((double)pieceScore / 64) * this->_pieceWeight;
	double mobilityTerm = ((double)mobilityScore / 20) * this->_mobilityWeight;
	double pMobilityTerm = ((double)pMobilityScore / 30) * this->_pMobilityWeight;
	double cornerTerm = cornerScore * this->_cornerWeight;
	double winTerm = winScore * this->_winWeight;

	// Adjust the win score.
	if (winScore == 1) {
		winTerm += myPieces;
	}
	else if (winScore == -1)
	{
		winTerm -= opponentPieces;
	}

	int result = (int)(pieceTerm + mobilityTerm + pMobilityTerm + cornerTerm + winTerm + 0.5);
	
	// Return the linear combination.
	return result;
	*/
}
