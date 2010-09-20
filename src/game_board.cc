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

GameBoard::GameBoard(bool same, bool plus, bool same_wall, bool elemental, Piece first_piece, std::vector<std::shared_ptr<const Card>> cards) :
	_current_piece(first_piece),
	_same(same),
	_plus(plus),
	_same_wall(same_wall),
	_elemental(elemental),
	_cards(cards),
	_squares_to_cards(),
	_owners(),
	_played_cards(),
	_move_history(),
	_card_history()
{
	this->_squares = Square::build_squares(3, 3);

	for (size_t i = 0; i < cards.size(); i++)
	{
		this->_cards[i] = cards[i];

		Piece owner = i < 5 ? PIECE_BLUE : PIECE_RED;
		this->_owners[cards[i]] = owner;
	}
}

GameBoard::GameBoard(const GameBoard & board) :
	_current_piece(board._current_piece),
	_same(board._same),
	_plus(board._plus),
	_same_wall(board._same_wall),
	_elemental(board._elemental),
	_cards(board._cards),
	_squares_to_cards(board._squares_to_cards),
	_owners(board._owners),
	_played_cards(board._played_cards),
	_move_history(),
	_card_history()
{ }

void GameBoard::move(std::shared_ptr<Move> move)
{
	if (!this->is_valid_move(move))
	{
		std::cerr << "Attempted to make invalid move: " << (*move) << std::endl;
		exit(1);
	}

	this->_squares_to_cards[move->square] = move->card;
	this->_played_cards.insert(move->card);
	this->_card_history.push(move->card);
	
	this->_execute_flip(move->square, NORTH);
	this->_execute_flip(move->square, SOUTH);
	this->_execute_flip(move->square, EAST);
	this->_execute_flip(move->square, WEST);

	this->_move_history.push(move);

	this->_current_piece = this->_current_piece == PIECE_BLUE ? PIECE_RED : PIECE_BLUE;
}

void GameBoard::unmove()
{
	std::shared_ptr<Move> move = this->_move_history.top();
	this->_move_history.pop();

	for (std::shared_ptr<const Card> card = this->_card_history.top(); card != move->card; card = this->_card_history.top())
	{
		this->_owners[card] = this->_owners[card] == PIECE_BLUE ? PIECE_RED : PIECE_BLUE;
		this->_card_history.pop();
	}

	this->_squares_to_cards[move->square] = std::shared_ptr<const Card>();
	this->_played_cards.erase(move->card);

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
		if (this->_owners[*iter] == piece)
			count++;
	}

	return count;
}

bool GameBoard::is_valid_move(std::shared_ptr<Move> move)
{
	return (!this->_squares_to_cards[move->square] && this->_owners[move->card] == this->_current_piece);
}

std::list<std::shared_ptr<Move>> GameBoard::get_valid_moves()
{
	std::list<std::shared_ptr<Move>> moves;

	for (auto card = this->_cards.begin(); card != this->_cards.end(); card++)
	{
		if (this->_played_cards.count(*card) == 0 && this->_owners[*card] == this->_current_piece)
		{
			for (auto square = this->_squares.begin(); square != this->_squares.end(); square++)
			{
				if (!this->_squares_to_cards[*square])
					moves.push_back(std::make_shared<Move>(Move(*square, *card)));
			}
		}
	}

	return moves;
}

std::shared_ptr<Move> GameBoard::get_move(std::shared_ptr<const Card> card, int row, int col)
{
	return std::make_shared<Move>(Move(this->_squares[row * 3 + col], card));
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

			std::shared_ptr<Move> last_move;
			if (!this->_move_history.empty())
				last_move = this->_move_history.top();

			if (last_move && last_move->square->row == row && last_move->square->col == col)
				boxRGBA(surface, col_offset, row_offset, col_offset + 99, row_offset + 99, 128, 128, 128, 255);
			else
				boxRGBA(surface, col_offset, row_offset, col_offset + 99, row_offset + 99, 64, 32, 0, 255);

			switch (this->_squares[row * 3 + col]->element)
			{
				case ELEMENT_NONE:
					break;

				case ELEMENT_FIRE:
					stringRGBA(surface, col_offset + 5, row_offset + 6, "Fire", 255, 255, 255, 255);
					break;

				default:
					break;
			}

			std::shared_ptr<const Card> card = this->_squares_to_cards[this->_squares[row * 3 + col]];

			if (card)
			{
				switch(this->_owners[card])
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

			if (this->_squares[row * 3 + col]->element != ELEMENT_NONE && this->_squares_to_cards[this->_squares[row * 3 + col]]->element != ELEMENT_NONE)
				elemental_adjustment += this->_squares[row * 3 + col]->element == this->_squares_to_cards[this->_squares[row * 3 + col]]->element ? 1 : -1;

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

	int index_blue = 0, index_red = 0;

	for (int i = 0; i < 10; i++)
	{
		if (this->_played_cards.count(this->_cards[i]) == 0)
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

void GameBoard::_execute_flip(std::shared_ptr<Square> square, Direction direction)
{
	std::shared_ptr<Square> target = square->get_neighbor(direction);

	if (!target)
		return;

	std::shared_ptr<const Card> card = this->_squares_to_cards[square];
	std::shared_ptr<const Card> target_card = this->_squares_to_cards[target];

	if (!target_card)
		return;

	if (this->_owners[target_card] != this->_current_piece)
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
			if (square->element != ELEMENT_NONE && card->element != ELEMENT_NONE)
				score += square->element == card->element ? 1 : -1;

			if (target->element != ELEMENT_NONE && target_card->element != ELEMENT_NONE)
				score -= square->element == card->element ? 1 : -1;
		}

		if (score > 0)
		{
			this->_card_history.push(target_card);
			this->_owners[target_card] = this->_current_piece;
		}

	}
}
