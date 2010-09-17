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

#include "card.hh"
#include "game_board.hh"
#include "player.hh"

#include "tripletriad.hh"

std::shared_ptr<TripleTriad> TripleTriad::_instance = std::shared_ptr<TripleTriad>();

TripleTriad::TripleTriad()
{
	// Create the graphics surface.
	this->_surface = SDL_SetVideoMode(524, 435, 0, SDL_ANYFORMAT);
	if (this->_surface == NULL)
	{
		std::cerr << "Unable to create video surface." << std::endl;
		SDL_Quit();
	}

	std::set<std::shared_ptr<Card>> cards;

	// Configure the cards.
	cards.insert(std::shared_ptr<Card>(new Card(PIECE_RED, 9, 7, 3, 6, ELEMENT_NONE)));
	cards.insert(std::shared_ptr<Card>(new Card(PIECE_RED, 1, 4, 1, 5, ELEMENT_NONE)));
	cards.insert(std::shared_ptr<Card>(new Card(PIECE_RED, 3, 3, 6, 7, ELEMENT_NONE)));
	cards.insert(std::shared_ptr<Card>(new Card(PIECE_RED, 3, 2, 1, 5, ELEMENT_NONE)));
	cards.insert(std::shared_ptr<Card>(new Card(PIECE_RED, 5, 1, 3, 1, ELEMENT_NONE)));
	
	cards.insert(std::shared_ptr<Card>(new Card(PIECE_BLUE, 6, 10, 4, 9, ELEMENT_NONE)));
	cards.insert(std::shared_ptr<Card>(new Card(PIECE_BLUE, 8, 10, 6, 5, ELEMENT_NONE)));
	cards.insert(std::shared_ptr<Card>(new Card(PIECE_BLUE, 9, 10, 2, 6, ELEMENT_NONE)));
	cards.insert(std::shared_ptr<Card>(new Card(PIECE_BLUE, 5, 8, 2, 10, ELEMENT_NONE)));
	cards.insert(std::shared_ptr<Card>(new Card(PIECE_BLUE, 9, 2, 8, 6, ELEMENT_NONE)));	
	
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
	Player *firstPlayer = new Player(this->_gameBoard, PIECE_BLUE, PIECE_RED);
	Player *secondPlayer = new Player(this->_gameBoard, PIECE_RED, PIECE_BLUE);

	while (!this->_gameBoard->get_valid_moves().empty())
	{
		this->_gameBoard->render(this->_surface);
		SDL_Flip(this->_surface);
		if (this->_gameBoard->get_current_piece() == PIECE_BLUE)
		{
			
			unsigned int start = SDL_GetTicks();
			firstPlayer->move(1500000);
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
	delete secondPlayer;
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
							getHumanCard = false;
							getHumanDestination = true;
						}
					}
					else if (getHumanDestination == true)
					{
						int row = (event.button.y - 121) / 101;
						int col = (event.button.x - 110) / 101;

						std::set<std::shared_ptr<Move>> moves = this->_gameBoard->get_valid_moves();

						std::set<std::shared_ptr<Move>>::iterator iter;
						int index = 0;

						for (iter = moves.begin(); iter != moves.end(); iter++)
						{
							if ((*iter)->row == row && (*iter)->col == col)
							{
								if (index == this->_cardChosen)
								{
									this->_gameBoard->move((*iter));
									getHumanDestination = false;
									break;
								}
								else
									index++;
							}
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
