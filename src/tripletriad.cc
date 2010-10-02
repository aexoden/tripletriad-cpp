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

#include <fstream>
#include <iostream>
#include <list>
#include <vector>

#include "card.hh"
#include "game_board.hh"
#include "move.hh"
#include "player.hh"
#include "square.hh"

#include "tripletriad.hh"

static Element parse_element(char ch)
{
	Element element;

	switch (ch)
	{
		case 'F':
			element = ELEMENT_FIRE;
			break;

		case 'I':
			element = ELEMENT_ICE;
			break;

		case 'T':
			element = ELEMENT_THUNDER;
			break;

		case 'P':
			element = ELEMENT_POISON;
			break;

		case 'E':
			element = ELEMENT_EARTH;
			break;

		case 'A':
			element = ELEMENT_WIND;
			break;

		case 'W':
			element = ELEMENT_WATER;
			break;

		case 'H':
			element = ELEMENT_HOLY;
			break;

		default:
			element = ELEMENT_NONE;
			break;
	}

	return element;
}

std::shared_ptr<TripleTriad> TripleTriad::_instance = std::shared_ptr<TripleTriad>();

TripleTriad::TripleTriad(const std::string & filename) :
	cards(10)
{
	// Create the graphics surface.
	this->_surface = SDL_SetVideoMode(524, 435, 0, SDL_ANYFORMAT);
	if (this->_surface == NULL)
	{
		std::cerr << "Unable to create video surface." << std::endl;
		SDL_Quit();
	}


	std::ifstream file(filename);

	if (!file.is_open())
		exit(1);

	std::string line;

	std::getline(file, line);
	Piece first_piece = line[0] == 'B' ? PIECE_BLUE : PIECE_RED;

	std::getline(file, line);
	bool same = line[0] == '1';
	bool plus = line[2] == '1';
	bool same_wall = line[4] == '1';
	bool elemental = line[6] == '1';

	std::getline(file, line);

	std::vector<Element> elements(9);

	for (int row = 0; row < 3; row++)
	{
		std::getline(file, line);

		for (int col = 0; col < 3; col++)
		{
			elements[row * 3 + col] = parse_element(line[col * 2]);
		}
	}

	std::getline(file, line);

	for (int i = 0; i < 10; i++)
	{
		std::getline(file, line);

		int top = line[0] == 'A' ? 10 : line[0] - 48;
		int bottom = line[2] == 'A' ? 10 : line[2] - 48;
		int left = line[4] == 'A' ? 10 : line[4] - 48;
		int right = line[6] == 'A' ? 10 : line[6] - 48;

		Element element = parse_element(line[8]);

		this->cards[i] = new Card(top, bottom, left, right, element);		

		if (i == 4)
			std::getline(file, line);
	}

	file.close();

	this->_gameBoard = new GameBoard(same, plus, same_wall, elemental, first_piece, elements, this->cards);
}

TripleTriad::~TripleTriad()
{
	delete this->_gameBoard;
}

std::shared_ptr<TripleTriad> TripleTriad::get_instance(const std::string & filename)
{
	if (!TripleTriad::_instance)
		TripleTriad::_instance = std::shared_ptr<TripleTriad>(new TripleTriad(filename));

	return TripleTriad::_instance;
}

void TripleTriad::run()
{
	Player *firstPlayer = new Player(std::shared_ptr<GameBoard>(this->_gameBoard), PIECE_BLUE, PIECE_RED);
	Player *secondPlayer = new Player(std::shared_ptr<GameBoard>(this->_gameBoard), PIECE_RED, PIECE_BLUE);

	bool blue_human = false;
	bool red_human = true;

	while (!this->_gameBoard->get_valid_moves().empty())
	{
		this->_gameBoard->render(this->_surface);
		SDL_Flip(this->_surface);
		if (!blue_human && this->_gameBoard->get_current_piece() == PIECE_BLUE)
		{
			unsigned int start = SDL_GetTicks();
			const Move * move = firstPlayer->get_move();
			this->_gameBoard->move(move);

			std::cout << "Time taken: " << ((SDL_GetTicks() - start) / 1000.0) << "s" << std::endl;
		}
		else if (!red_human && this->_gameBoard->get_current_piece() == PIECE_RED)
		{
			unsigned int start = SDL_GetTicks();
			const Move * move = secondPlayer->get_move();
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
	
	std::cout << "Final score: Blue: " << firstPieces << "   Red: " << secondPieces << std::endl;

	// Loop until the user exits.
	this->_gameBoard->render(this->_surface);
	SDL_Flip(this->_surface);
	while (true)
	{
		this->checkEvent(true);
	}
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
							if (event.button.x > 414 && event.button.x < 514)
								this->_cardChosen = 0;
							else
								this->_cardChosen = 5;

							this->_cardChosen += (event.button.y - 10) / 80;

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

						const Move * move = this->_gameBoard->get_move(cards[this->_cardChosen], row, col);

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

	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
		exit(1);
	}

	std::shared_ptr<TripleTriad> tripletriad = TripleTriad::get_instance(std::string(argv[1]));
	tripletriad->run();

	return 0;
}
