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

#include <map>

#include "SDL_gfxPrimitives.h"

#include "card.hh"
#include "game_board.hh"
#include "move.hh"
#include "square.hh"

GameBoard::GameBoard(bool same, bool plus, bool same_wall, bool elemental, Piece first_piece, std::vector<Element> elements, std::vector<const Card *> cards) :
	_current_piece(first_piece),
	_same(same),
	_plus(plus),
	_same_wall(same_wall),
	_elemental(elemental),
	_cards(cards),
	_moves(9 * 10),
	_squares_to_cards(9),
	_owners(10),
	_played_cards(10, false),
	_move_history(),
	_card_history()
{
	this->_squares = Square::build_squares(3, 3, elements);

	for (size_t i = 0; i < cards.size(); i++)
	{
		this->_cards[i] = cards[i];

		Piece owner = i < 5 ? PIECE_BLUE : PIECE_RED;
		this->_owners[cards[i]->id] = owner;
	}

	for (auto card = this->_cards.begin(); card != this->_cards.end(); card++)
	{
		for (auto square = this->_squares.begin(); square != this->_squares.end(); square++)
		{
			this->_moves[(*card)->id * 9 + (*square)->id] = new Move(*square, *card);
		}
	}
}

GameBoard::GameBoard(const GameBoard & board) :
	_current_piece(board._current_piece),
	_same(board._same),
	_plus(board._plus),
	_same_wall(board._same_wall),
	_elemental(board._elemental),
	_cards(board._cards),
	_moves(board._moves),
	_squares_to_cards(board._squares_to_cards),
	_owners(board._owners),
	_played_cards(board._played_cards),
	_move_history(),
	_card_history()
{ }

void GameBoard::move(const Move * const move)
{
	if (!this->is_valid_move(move))
	{
		std::cerr << "Attempted to make invalid move: " << (*move) << std::endl;
		exit(1);
	}

	this->_squares_to_cards[move->square->id] = move->card;
	this->_played_cards[move->card->id] = true;
	this->_card_history.push(move->card);

	if (this->_same || this->_plus)
	{
		bool north = false, south = false, east = false, west = false;
		bool combo = false;

		if (this->_same)
		{
			bool north_same = this->_check_same(move->square, NORTH);
			bool south_same = this->_check_same(move->square, SOUTH);
			bool west_same = this->_check_same(move->square, WEST);
			bool east_same = this->_check_same(move->square, EAST);

			if (north_same && east_same)
				north = east = true;

			if (north_same && south_same)
				north = south = true;

			if (north_same && west_same)
				north = west = true;

			if (east_same && west_same)
				east = west = true;

			if (east_same && south_same)
				east = south = true;

			if (west_same && south_same)
				west = south = true;

			if (north || south || east || west)
				combo = true;
		}

		if (this->_plus)
		{
			int north_value = this->_check_plus(move->square, NORTH);
			int south_value = this->_check_plus(move->square, SOUTH);
			int east_value = this->_check_plus(move->square, EAST);
			int west_value = this->_check_plus(move->square, WEST);

			if (north_value > 0 && north_value == east_value)
				north = east = true;

			if (east_value > 0 && east_value == south_value)
				east = south = true;

			if (south_value > 0 && south_value == west_value)
				south = west = true;

			if (west_value > 0 && west_value == north_value)
				west = north = true;

			if (north_value > 0 && north_value == south_value)
				north = south = true;

			if (east_value > 0 && east_value == west_value)
				east = west = true;

			if (north || south || east || west)
				combo = true;
		}

		if (north) {
			this->_execute_basic(move->square->get_neighbor(NORTH), true);
		} else if (this->_execute_flip(move->square, NORTH) && combo) {
			this->_execute_basic(move->square->get_neighbor(NORTH), false);
		}
		
		if (south) {
			this->_execute_basic(move->square->get_neighbor(SOUTH), true);
		} else if (this->_execute_flip(move->square, SOUTH) && combo) {
			this->_execute_basic(move->square->get_neighbor(SOUTH), false);
		}
		
		if (east) {
			this->_execute_basic(move->square->get_neighbor(EAST), true);
		} else if (this->_execute_flip(move->square, EAST) && combo) {
			this->_execute_basic(move->square->get_neighbor(EAST), false);
		}
		
		if (west) {
			this->_execute_basic(move->square->get_neighbor(WEST), true);
		} else if (this->_execute_flip(move->square, WEST) && combo) {
			this->_execute_basic(move->square->get_neighbor(WEST), false);
		}		
	} else {
		this->_execute_flip(move->square, NORTH);
		this->_execute_flip(move->square, SOUTH);
		this->_execute_flip(move->square, EAST);
		this->_execute_flip(move->square, WEST);
	}

	this->_move_history.push(move);

	this->_current_piece = this->_current_piece == PIECE_BLUE ? PIECE_RED : PIECE_BLUE;
}

void GameBoard::unmove()
{
	const Move * move = this->_move_history.top();
	this->_move_history.pop();

	for (const Card * card = this->_card_history.top(); card != move->card; card = this->_card_history.top())
	{
		this->_owners[card->id] = this->_owners[card->id] == PIECE_BLUE ? PIECE_RED : PIECE_BLUE;
		this->_card_history.pop();
	}

	this->_squares_to_cards[move->square->id] = NULL;
	this->_played_cards[move->card->id] = false;

	this->_card_history.pop();

	this->_current_piece = this->_current_piece == PIECE_BLUE ? PIECE_RED : PIECE_BLUE;
}

Piece GameBoard::get_current_piece()
{
	return this->_current_piece;
}

int GameBoard::get_score(Piece piece)
{
	int count = 0;

	for (auto iter = this->_cards.begin(); iter != this->_cards.end(); iter++)
	{
		if (this->_owners[(*iter)->id] == piece)
			count++;
	}

	return count;
}

bool GameBoard::is_valid_move(const Move * move)
{
	return (!this->_squares_to_cards[move->square->id] && this->_owners[move->card->id] == this->_current_piece);
}

std::list<const Move *> GameBoard::get_valid_moves()
{
	std::list<const Move *> moves;
	
	for (auto card = this->_cards.begin(); card != this->_cards.end(); card++)
	{
		if (this->_owners[(*card)->id] == this->_current_piece && !this->_played_cards[(*card)->id])
		{
			for (auto square = this->_squares.begin(); square != this->_squares.end(); square++)
			{
				if (!this->_squares_to_cards[(*square)->id])
				{
					moves.push_back(this->_moves[(*card)->id * 9 + (*square)->id]);
				}
			}
		}
	}

	return moves;
}

const Move * GameBoard::get_move(const Card * card, int row, int col)
{
	const Square * square = this->_squares[row * 3 + col];

	return this->_moves[card->id * 9 + square->id];
}

void GameBoard::render(SDL_Surface * surface)
{
	boxRGBA(surface, 0, 0, 523, 434, 128, 64, 0, 255);

	boxRGBA(surface, 110, 121, 110 + 303, 121 + 303, 0, 0, 0, 255);

	for (int row = 0; row < 3; row++)
	{
		for (int col = 0; col < 3; col++)
		{
			int col_offset = 110 + (col + 1) + col * 100;
			int row_offset = 121 + (row + 1) + row * 100;

			const Move * last_move = NULL;
			if (!this->_move_history.empty())
				last_move = this->_move_history.top();

			if (last_move && last_move->square->row == row && last_move->square->col == col)
				boxRGBA(surface, col_offset, row_offset, col_offset + 99, row_offset + 99, 128, 128, 128, 255);
			else
				boxRGBA(surface, col_offset, row_offset, col_offset + 99, row_offset + 99, 64, 32, 0, 255);

			if (this->_elemental)
			{
			switch (this->_squares[row * 3 + col]->element)
			{
				case ELEMENT_NONE:
					break;

				case ELEMENT_FIRE:
					stringRGBA(surface, col_offset + 5, row_offset + 6, "Fire", 255, 255, 255, 255);
					break;

				case ELEMENT_ICE:
					stringRGBA(surface, col_offset + 5, row_offset + 6, "Ice", 255, 255, 255, 255);
					break;

				case ELEMENT_THUNDER:
					stringRGBA(surface, col_offset + 5, row_offset + 6, "Thunder", 255, 255, 255, 255);
					break;

				case ELEMENT_POISON:
					stringRGBA(surface, col_offset + 5, row_offset + 6, "Poison", 255, 255, 255, 255);
					break;

				case ELEMENT_EARTH:
					stringRGBA(surface, col_offset + 5, row_offset + 6, "Earth", 255, 255, 255, 255);
					break;

				case ELEMENT_WIND:
					stringRGBA(surface, col_offset + 5, row_offset + 6, "Wind", 255, 255, 255, 255);
					break;

				case ELEMENT_WATER:
					stringRGBA(surface, col_offset + 5, row_offset + 6, "Water", 255, 255, 255, 255);
					break;

				case ELEMENT_HOLY:
					stringRGBA(surface, col_offset + 5, row_offset + 6, "Holy", 255, 255, 255, 255);
					break;
			}
			}

			const Card * card = this->_squares_to_cards[this->_squares[row * 3 + col]->id];

			if (card)
			{
				switch(this->_owners[card->id])
				{
					case PIECE_BLUE:
						boxRGBA(surface, col_offset + 5, row_offset + 20, col_offset + 5 + 89, row_offset + 20 + 74, 0, 0, 0, 255);
						boxRGBA(surface, col_offset + 6, row_offset + 21, col_offset + 6 + 87, row_offset + 21 + 72, 0, 0, 128, 255);
						break;

					case PIECE_RED:
						boxRGBA(surface, col_offset + 5, row_offset + 20, col_offset + 5 + 89, row_offset + 20 + 74, 0, 0, 0, 255);
						boxRGBA(surface, col_offset + 6, row_offset + 21, col_offset + 6 + 87, row_offset + 21 + 72, 128, 0, 0, 255);
						break;

					default:
						break;
				}

				card->render(surface, col_offset + 6, row_offset + 21);
			}

			int elemental_adjustment = 0;

			if (this->_squares[row * 3 + col]->element != ELEMENT_NONE && this->_squares_to_cards[this->_squares[row * 3 + col]->id])
				elemental_adjustment += this->_squares[row * 3 + col]->element == this->_squares_to_cards[this->_squares[row * 3 + col]->id]->element ? 1 : -1;

			if (this->_elemental)
			{
				switch (elemental_adjustment)
				{
					case 1:
						stringRGBA(surface, col_offset + 40, row_offset + 75, "+1", 255, 255, 255, 255);
						break;

					case -1:
						stringRGBA(surface, col_offset + 40, row_offset + 75, "-1", 255, 255, 255, 255);
						break;
				}
			}
		}
	}

	int index_blue = 0, index_red = 0;

	for (int i = 0; i < 10; i++)
	{
		if (!this->_played_cards[this->_cards[i]->id])
		{
			if (i < 5)
			{
				boxRGBA(surface, 414 + 10, index_blue * 85 + 10, 414 + 99, index_blue * 85 + 84, 0, 0, 0, 255);
				boxRGBA(surface, 414 + 11, index_blue * 85 + 11, 414 + 98, index_blue * 85 + 83, 0, 0, 128, 255);

				this->_cards[i]->render(surface, 414 + 11, index_blue * 85 + 11);

				index_blue++;			
			}
			else
			{
				boxRGBA(surface, 10, index_red * 85 + 10, 99, index_red * 85 + 84, 0, 0, 0, 255);
				boxRGBA(surface, 11, index_red * 85 + 11, 98, index_red * 85 + 83, 128, 0, 0, 255);

				this->_cards[i]->render(surface, 11, index_red * 85 + 11);

				index_red++;
			}
		}
		else
		{
			if (i < 5)
				index_blue++;
			else
				index_red++;
		}
	}
}

void GameBoard::_execute_basic(const Square * square, bool check)
{
//	if (square && (!check || this->_owners[this->_squares_to_cards[square->id]->id] != this->_current_piece))
	if (square && (!check || this->_owners[this->_squares_to_cards[square->id]->id] != this->_current_piece))
	{
		const Card * target_card = this->_squares_to_cards[square->id];

		if (this->_owners[target_card->id] != this->_current_piece)
		{
			this->_card_history.push(target_card);
			this->_owners[target_card->id] = this->_current_piece;
		}

		if (this->_execute_flip(square, NORTH))
			this->_execute_basic(square->get_neighbor(NORTH), false);

		if (this->_execute_flip(square, SOUTH))
			this->_execute_basic(square->get_neighbor(SOUTH), false);

		if (this->_execute_flip(square, EAST))
			this->_execute_basic(square->get_neighbor(EAST), false);

		if (this->_execute_flip(square, WEST))
			this->_execute_basic(square->get_neighbor(WEST), false);
	}
}

bool GameBoard::_execute_flip(const Square * square, Direction direction)
{
	const Square * target = square->get_neighbor(direction);

	if (!target)
		return false;

	const Card * card = this->_squares_to_cards[square->id];
	const Card * target_card = this->_squares_to_cards[target->id];

	if (!target_card)
		return false;

	if (this->_owners[target_card->id] != this->_current_piece)
	{
		int score = 0;

		switch (direction)
		{
			case NORTH:
				score += card->top;
				score -= target_card->bottom;
				break;

			case SOUTH:
				score += card->bottom;
				score -= target_card->top;
				break;

			case EAST:
				score += card->right;
				score -= target_card->left;
				break;

			case WEST:
				score += card->left;
				score -= target_card->right;
				break;
		}

		if (this->_elemental)
		{
			if (square->element != ELEMENT_NONE)
				score += square->element == card->element ? 1 : -1;

			if (target->element != ELEMENT_NONE)
				score -= target->element == target_card->element ? 1 : -1;
		}

		if (score > 0)
		{
			this->_card_history.push(target_card);
			this->_owners[target_card->id] = this->_current_piece;
			return true;
		}

	}

	return false;
}

int GameBoard::_check_plus(const Square * square, Direction direction)
{
	const Card * card = this->_squares_to_cards[square->id];

	const Square * target = square->get_neighbor(direction);
	const Card * target_card = NULL;

	if (target)
		target_card = this->_squares_to_cards[target->id];

	if (!target_card)
		return 0;

	switch(direction)
	{
		case NORTH:
			return card->top + target_card->bottom;

		case SOUTH:
			return card->bottom + target_card->top;

		case EAST:
			return card->right + target_card->left;

		case WEST:
			return card->left + target_card->right;
	}

	return 0;
}

bool GameBoard::_check_same(const Square * square, Direction direction)
{
	const Card * card = this->_squares_to_cards[square->id];

	const Square * target = square->get_neighbor(direction);
	const Card * target_card = NULL;

	if (target)
	{
		target_card = this->_squares_to_cards[target->id];

		if (!target_card)
			return false;
	}

	int value;
	int target_value;

	if (!target_card && !this->_same_wall)
		return false;

	switch (direction)
	{
		case NORTH:
			value = card->top;
			target_value = target_card ? target_card->bottom : 10;
			break;

		case SOUTH:
			value = card->bottom;
			target_value = target_card ? target_card->top : 10;
			break;

		case EAST:
			value = card->right;
			target_value = target_card ? target_card->left : 10;
			break;

		case WEST:
			value = card->left;
			target_value = target_card ? target_card->right : 10;
			break;
	}

	return value == target_value;
}
