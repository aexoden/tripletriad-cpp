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

GameBoard::GameBoard(bool same, bool plus, bool same_wall, bool elemental, Piece first_piece, std::set<std::shared_ptr<Card>> cards) :
	_current_piece(first_piece),
	_same(same),
	_plus(plus),
	_same_wall(same_wall),
	_elemental(elemental),
	_cards(cards),
	_move_history(),
	_card_history()
{
	this->_board = Square::build_squares(3, 3);
}

GameBoard::GameBoard(const GameBoard & board) :
	_current_piece(board._current_piece),
	_same(board._same),
	_plus(board._plus),
	_same_wall(board._same_wall),
	_elemental(board._elemental),
	_cards(),
	_move_history(),
	_card_history()
{
	std::map<std::shared_ptr<Card>, std::shared_ptr<Card>> card_map;

	for (std::set<std::shared_ptr<Card>>::iterator iter = board._cards.begin(); iter != board._cards.end(); iter++)
	{
		std::shared_ptr<Card> card(new Card(**iter));

		card_map[*iter] = card;

		this->_cards.insert(card);
	}

	this->_board = Square::build_squares(3, 3);

	for (int row = 0; row < 3; row++)
	{
		for (int col = 0; col < 3; col++)
		{
			std::shared_ptr<Card> old_card = board._board[row * 3 + col]->get_card();

			if (old_card)
			{
				std::shared_ptr<Card> new_card = card_map[old_card];
				this->_board[row * 3 + col]->set_card(new_card);

				new_card->set_square(this->_board[row * 3 + col]);
			}
		}
	}
}

void GameBoard::move(std::shared_ptr<Move> move)
{
	if (!this->is_valid_move(move))
	{
		std::cerr << "Attempted to make invalid move: " << (*move) << std::endl;
		exit(1);
	}

	// TODO: Elemental is assumed to always be on at the moment. Since the board won't have any elementals if the
	// elemental rule is disabled, this doesn't matter practically, but it makes the elemental rule parameter useless.
	std::set<std::shared_ptr<Card>> flipped_cards;

	move->square->set_card(move->card);
	move->card->set_square(move->square);

	std::shared_ptr<const Square> target_square;

	if (target_square = move->square->get_neighbor(WEST))
	{
		std::shared_ptr<Card> target_card = target_square->get_card();

		if (target_card && target_card->get_owner() != this->_current_piece &&
			move->card->left + move->square->get_elemental_adjustment() > target_card->right + target_square->get_elemental_adjustment())
		{
				target_card->set_owner(this->_current_piece);
				flipped_cards.insert(target_card);
		}
	}

	if (target_square = move->square->get_neighbor(EAST))
	{
		std::shared_ptr<Card> target_card = target_square->get_card();

		if (target_card && target_card->get_owner() != this->_current_piece &&
			move->card->right + move->square->get_elemental_adjustment() > target_card->left + target_square->get_elemental_adjustment())
		{
				target_card->set_owner(this->_current_piece);
				flipped_cards.insert(target_card);
		}
	}

	if (target_square = move->square->get_neighbor(NORTH))
	{
		std::shared_ptr<Card> target_card = target_square->get_card();

		if (target_card && target_card->get_owner() != this->_current_piece &&
			move->card->top + move->square->get_elemental_adjustment() > target_card->bottom + target_square->get_elemental_adjustment())
		{
				target_card->set_owner(this->_current_piece);
				flipped_cards.insert(target_card);
		}
	}

	if (target_square = move->square->get_neighbor(SOUTH))
	{
		std::shared_ptr<Card> target_card = target_square->get_card();

		if (target_card && target_card->get_owner() != this->_current_piece &&
			move->card->bottom + move->square->get_elemental_adjustment() > target_card->top + target_square->get_elemental_adjustment())
		{
				target_card->set_owner(this->_current_piece);
				flipped_cards.insert(target_card);
		}
	}

	this->_move_history.push(move);
	this->_card_history.push(flipped_cards);

	this->_current_piece = this->_current_piece == PIECE_BLUE ? PIECE_RED : PIECE_BLUE;
}

void GameBoard::unmove()
{
	std::set<std::shared_ptr<Card>> flipped_cards = this->_card_history.top();

	for (std::set<std::shared_ptr<Card>>::iterator iter = flipped_cards.begin(); iter != flipped_cards.end(); iter++)
	{
		(*iter)->set_owner(this->_current_piece);
	}

	std::shared_ptr<Move> move = this->_move_history.top();

	move->square->get_card()->set_square(std::shared_ptr<Square>());
	move->square->set_card(std::shared_ptr<Card>());

	this->_card_history.pop();
	this->_move_history.pop();

	this->_current_piece = this->_current_piece == PIECE_BLUE ? PIECE_RED : PIECE_BLUE;
}

Piece GameBoard::get_current_piece()
{
	return this->_current_piece;
}

int GameBoard::get_score(Piece piece)
{
	std::set<std::shared_ptr<Card>>::iterator iter;
	int count = 0;

	for (iter = this->_cards.begin(); iter != this->_cards.end(); iter++)
	{
		if ((*iter)->get_owner() == piece)
			count++;

	}

	return count;
}

bool GameBoard::is_valid_move(std::shared_ptr<Move> move)
{
	return (!move->square->get_card() && move->card->get_owner() == this->_current_piece);
}

std::list<std::shared_ptr<Move>> GameBoard::get_valid_moves()
{
	std::set<std::shared_ptr<Card>> cards = this->_cards;
	std::list<std::shared_ptr<Move>> moves;

	while (!cards.empty())
	{
		int best_score = 0;
		std::shared_ptr<Card> best_card;

		for (auto iter = cards.begin(); iter != cards.end(); iter++)
		{
			int score = (*iter)->top + (*iter)->left + (*iter)->right + (*iter)->bottom;

			if (score > best_score)
			{
				best_score = score;
				best_card = (*iter);
			}
		}

		cards.erase(best_card);

		if (!best_card->get_square() && best_card->get_owner() == this->_current_piece)
		{
			for (auto iter = this->_board.begin(); iter != this->_board.end(); iter++)
			{
				std::shared_ptr<Move> move(new Move(*iter, best_card));

				if (this->is_valid_move(move))
					moves.push_back(move);
			}
		}
	}

	return moves;
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

			switch (this->_board[row * 3 + col]->element)
			{
				case ELEMENT_NONE:
					break;

				case ELEMENT_FIRE:
					stringRGBA(surface, col_offset + 5, row_offset + 6, "Fire", 255, 255, 255, 255);
					break;

				default:
					break;
			}

			std::shared_ptr<Card> card = this->_board[row * 3 + col]->get_card();

			if (card)
			{
				switch(card->get_owner())
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

			switch (this->_board[row * 3 + col]->get_elemental_adjustment())
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

	for (std::set<std::shared_ptr<Card>>::iterator iter = this->_cards.begin(); iter != this->_cards.end(); iter++)
	{
		if (!(*iter)->get_square())
		{
			if ((*iter)->get_owner() == PIECE_BLUE)
			{
				boxRGBA(surface, 414 + 10, index_blue * 85 + 10, 414 + 99, index_blue * 85 + 84, 0, 0, 0, 255);
				boxRGBA(surface, 414 + 11, index_blue * 85 + 11, 414 + 98, index_blue * 85 + 83, 0, 0, 128, 255);

				(*iter)->render(surface, 414 + 11, index_blue * 85 + 11);

				index_blue++;			
			}
			else
			{
				boxRGBA(surface, 10, index_red * 85 + 10, 99, index_red * 85 + 84, 0, 0, 0, 255);
				boxRGBA(surface, 11, index_red * 85 + 11, 98, index_red * 85 + 83, 128, 0, 0, 255);

				(*iter)->render(surface, 11, index_red * 85 + 11);

				index_red++;
			}
		}
	}
}
