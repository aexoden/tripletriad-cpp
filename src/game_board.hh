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
#include <map>
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
		GameBoard(bool same, bool plus, bool same_wall, bool elemental, Piece first_piece, std::vector<std::shared_ptr<const Card>> cards);
		GameBoard(const GameBoard & board);

		void move(const Move * move);
		void unmove();

		Piece get_current_piece();

		int get_score(Piece piece);

		bool is_valid_move(const Move * move);

		std::list<const Move *> get_valid_moves();

		const Move * get_move(std::shared_ptr<const Card> card, int row, int col);

		void render(SDL_Surface * surface);
	private:
		void _execute_flip(std::shared_ptr<Square> square, Direction direction);

		Piece _current_piece;
		bool _same, _plus, _same_wall, _elemental;

		std::vector<std::shared_ptr<const Card>> _cards;
		std::vector<std::shared_ptr<Square>> _squares;

		std::vector<const Move *> _moves;

		std::vector<std::shared_ptr<const Card>> _squares_to_cards;
		std::vector<Piece> _owners;
		std::set<std::shared_ptr<const Card>> _played_cards;

		std::stack<const Move *> _move_history;
		std::stack<std::shared_ptr<const Card>> _card_history;
};

#endif
