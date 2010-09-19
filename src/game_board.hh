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

#ifndef TRIPLETRIAD_GAME_BOARD_HH
#define TRIPLETRIAD_GAME_BOARD_HH

#include <list>
#include <memory>
#include <set>
#include <stack>
#include <vector>

#include "SDL.h"

#include "common.hh"

class Card;
class Move;
class Square;

class GameBoard
{
	public:
		GameBoard(bool same, bool plus, bool same_wall, bool elemental, Piece first_piece, std::set<std::shared_ptr<Card>> cards);
		GameBoard(const GameBoard & board);

		void move(std::shared_ptr<Move> move);
		void unmove();

		Piece get_current_piece();

		int get_score(Piece piece);

		bool is_valid_move(std::shared_ptr<Move> move);

		std::list<std::shared_ptr<Move>> get_valid_moves();

		void render(SDL_Surface * surface);
	private:
		Piece _current_piece;
		bool _same, _plus, _same_wall, _elemental;

		std::set<std::shared_ptr<Card>> _cards;

		std::vector<std::shared_ptr<Square>> _board;

		std::stack<std::shared_ptr<Move>> _move_history;
		std::stack<std::set<std::shared_ptr<Card>>> _card_history;
};

#endif
