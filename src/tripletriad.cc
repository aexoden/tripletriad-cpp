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

#include <iostream>
#include <list>
#include <vector>

#include "card.hh"
#include "game_board.hh"
#include "move.hh"
#include "player.hh"
#include "square.hh"

#include "tripletriad.hh"

std::shared_ptr<TripleTriad> TripleTriad::_instance = std::shared_ptr<TripleTriad>();

TripleTriad::TripleTriad() :
	cards(10)
{
	// Create the graphics surface.
	this->_surface = SDL_SetVideoMode(524, 435, 0, SDL_ANYFORMAT);
	if (this->_surface == NULL)
	{
		std::cerr << "Unable to create video surface." << std::endl;
		SDL_Quit();
	}

	// Configure the cards.
	cards[0] = std::shared_ptr<const Card>(new Card(6, 10, 4, 9, ELEMENT_NONE));
	cards[1] = std::shared_ptr<const Card>(new Card(8, 10, 6, 5, ELEMENT_NONE));
	cards[2] = std::shared_ptr<const Card>(new Card(9, 10, 2, 6, ELEMENT_NONE));
	cards[3] = std::shared_ptr<const Card>(new Card(5, 8, 2, 10, ELEMENT_NONE));
	cards[4] = std::shared_ptr<const Card>(new Card(9, 2, 8, 6, ELEMENT_NONE));	

	cards[5] = std::shared_ptr<const Card>(new Card(9, 7, 3, 6, ELEMENT_NONE));
	cards[6] = std::shared_ptr<const Card>(new Card(1, 4, 1, 5, ELEMENT_NONE));
	cards[7] = std::shared_ptr<const Card>(new Card(3, 3, 6, 7, ELEMENT_NONE));
	cards[8] = std::shared_ptr<const Card>(new Card(3, 2, 1, 5, ELEMENT_NONE));
	cards[9] = std::shared_ptr<const Card>(new Card(5, 1, 3, 1, ELEMENT_NONE));
	
	
	// Configure the game board and its rules.
	// GameBoard(start_piece, blue_cards, red_cards, same, plus, same wall, elemental)
	this->_gameBoard = new GameBoard(false, false, false, false, PIECE_RED, cards);
}

TripleTriad::~TripleTriad()
{
	delete this->_gameBoard;
}

std::shared_ptr<TripleTriad> TripleTriad::get_instance()
{
	if (!TripleTriad::_instance)
		TripleTriad::_instance = std::shared_ptr<TripleTriad>(new TripleTriad());

	return TripleTriad::_instance;
}

void TripleTriad::run()
{
	Player *firstPlayer = new Player(std::shared_ptr<GameBoard>(this->_gameBoard), PIECE_BLUE, PIECE_RED);
//	Player *secondPlayer = new Player(this->_gameBoard, PIECE_RED, PIECE_BLUE);

	while (!this->_gameBoard->get_valid_moves().empty())
	{
		this->_gameBoard->render(this->_surface);
		SDL_Flip(this->_surface);
		if (this->_gameBoard->get_current_piece() == PIECE_BLUE)
		{
			unsigned int start = SDL_GetTicks();
			std::shared_ptr<Move> move = firstPlayer->get_move();
			this->_gameBoard->move(move);

			std::cout << "Time taken: " << ((SDL_GetTicks() - start) / 1000.0) << "s" << std::endl;
		}
		else
		{
			if (this->checkEvent(true) == true)
			{
				break;
			}
		}
	}

	int firstPieces = this->_gameBoard->get_score(PIECE_BLUE);
	int secondPieces = this->_gameBoard->get_score(PIECE_RED);
	
//	this->_gameBoard->printBoard();
	std::cout << "Final score: Blue: " << firstPieces << "   Red: " << secondPieces << std::endl;

	// Loop until the user exits.
	this->_gameBoard->render(this->_surface);
	SDL_Flip(this->_surface);
	while (true)
	{
		this->checkEvent(true);
	}

	delete firstPlayer;
//	delete secondPlayer;
}

bool TripleTriad::checkEvent(bool getHumanCard)
{
	SDL_Event event;
	
	bool getHumanDestination = false;

	do
	{
		if (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
					exit(0);
					break;

				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_q)
					{
						exit(0);
					}
					break;

				case SDL_MOUSEBUTTONUP:
					if (getHumanCard == true)
					{
						if ((event.button.x >= 10 && event.button.x < 100) || (event.button.x >= 414 && event.button.x < 514))
						{
							this->_cardChosen = (event.button.y - 10) / 80;
							std::cout << "Player chose card: " << this->_cardChosen << std::endl;
							getHumanCard = false;
							getHumanDestination = true;
						}
					}
					else if (getHumanDestination == true)
					{
						int row = (event.button.y - 121) / 101;
						int col = (event.button.x - 110) / 101;

						std::cout << "Player chose: " << row << ", " << col << std::endl;

						std::shared_ptr<Move> move = this->_gameBoard->get_move(cards[this->_cardChosen + 5], row, col);

						if (!move)
						{
							getHumanDestination = false;
							getHumanCard = true;
						}
						else
						{
							this->_gameBoard->move(move);
							getHumanDestination = false;
						}
					}
					break;

				default:
					break;
			}
		}
	}
	while (getHumanDestination == true || getHumanCard == true);

	return false;
}

int main(int argc, char* argv[])
{
	std::cout << "Triple Triad " << VERSION << std::endl;

	// Initialize SDL graphics.
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cerr << "Video initialization failed: " << SDL_GetError() << std::endl;
		SDL_Quit();
	}

	// Register the exit functions.
	if (atexit(SDL_Quit) != 0)
	{
		std::cerr << "Error registering exit handler." << std::endl;
		SDL_Quit();
	}

	std::shared_ptr<TripleTriad> tripletriad = TripleTriad::get_instance();
	tripletriad->run();

	return 0;
}
