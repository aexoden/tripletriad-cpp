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
#include <set>

#include "common.hh"
#include "game_board.hh"
#include "move.hh"
#include "player.hh"
#include "tripletriad.hh"

Player::Player(std::shared_ptr<GameBoard> board, Piece my_piece, Piece opponent_piece) :
	_board(board),
	_test_board(std::shared_ptr<GameBoard>()),
	_my_piece(my_piece),
	_opponent_piece(opponent_piece)
{ }

std::shared_ptr<Move> Player::get_move()
{
	this->_test_board = this->_board;

	std::shared_ptr<Move> best_move;

	bool complete = false;

	for (int ply = 1; !complete; ply++)
	{
		int positions = 0;
		int best_score = std::numeric_limits<int>::min();

		std::list<std::shared_ptr<Move>> moves  = this->_test_board->get_valid_moves();

		for (auto iter = moves.begin(); iter != moves.end(); iter++)
		{
			this->_test_board->move(*iter);

			complete = true;
			int score = this->_search_minimax(ply - 1, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), complete, positions);

			this->_test_board->unmove();

			if (score > best_score)
			{
				best_score = score;
				best_move = *iter;
			}

			positions++;
		}

		std::cout << std::left;
		std::cout << std::setw(12) << "Search Ply:" << std::setw(4) << ply;
		std::cout << std::setw(11) << "Positions:" << std::setw(12) << positions;
		std::cout << std::setw(6) << "Move:" << std::setw(30) << (*best_move);
		std::cout << std::setw(10) << "Utility:" << std::setw(10) << best_score;
		std::cout << std::endl;
	}

	return best_move;
}

int Player::_search_minimax(int max_ply, int alpha, int beta, bool & complete, int & positions)
{
	if (positions % 1000 == 0)
		TripleTriad::get_instance()->checkEvent(false);

	std::list<std::shared_ptr<Move>> moves = this->_test_board->get_valid_moves();

	if (max_ply == 0 && !moves.empty())
		complete = false;

	if (max_ply == 0 || moves.empty())
		return this->_evaluate();

	for (auto iter = moves.begin(); iter != moves.end(); iter++)
	{
		this->_test_board->move(*iter);
		int score = this->_search_minimax(max_ply - 1, alpha, beta, complete, positions);
		this->_test_board->unmove();

		if (this->_test_board->get_current_piece() == this->_my_piece)
		{
			if (score >= beta)
				return beta;

			if (score > alpha)
				alpha = score;
		}
		else
		{
			if (score <= alpha)
				return alpha;

			if (score < beta)
				beta = score;
		}

		positions++;
	}

	if (this->_test_board->get_current_piece() == this->_my_piece)
		return alpha;
	else
		return beta;
}

int Player::_evaluate()
{
	return this->_test_board->get_score(this->_my_piece) - this->_test_board->get_score(this->_opponent_piece);
}
