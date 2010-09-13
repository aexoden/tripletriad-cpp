#include <iostream>
#include <list>

#include "game_board.hh"
#include "player.hh"

#include "tripletriad.hh"

TripleTriad::TripleTriad()
{
	// Create the graphics surface.
	this->_surface = SDL_SetVideoMode(524, 435, 0, SDL_ANYFORMAT);
	if (this->_surface == NULL)
	{
		std::cerr << "Unable to create video surface." << std::endl;
		SDL_Quit();
	}

	std::list<Card*> redCards;
	std::list<Card*> blueCards;

	// Configure the cards.
	redCards.push_back(new Card(9, 7, 3, 6, ELEMENT_NONE));
	redCards.push_back(new Card(1, 4, 1, 5, ELEMENT_NONE));
	redCards.push_back(new Card(3, 3, 6, 7, ELEMENT_NONE));
	redCards.push_back(new Card(3, 2, 1, 5, ELEMENT_NONE));
	redCards.push_back(new Card(5, 1, 3, 1, ELEMENT_NONE));
	
	blueCards.push_back(new Card(6, 10, 4, 9, ELEMENT_NONE));
	blueCards.push_back(new Card(8, 10, 6, 5, ELEMENT_NONE));
	blueCards.push_back(new Card(9, 10, 2, 6, ELEMENT_NONE));
	blueCards.push_back(new Card(5, 8, 2, 10, ELEMENT_NONE));
	blueCards.push_back(new Card(9, 2, 8, 6, ELEMENT_NONE));	
	
	// Configure the game board and its rules.
	// GameBoard(start_piece, blue_cards, red_cards, same, plus, same wall, elemental)
	this->_gameBoard = new GameBoard(PIECE_BLUE, blueCards, redCards, true, false, false, false);
}

TripleTriad::~TripleTriad()
{
	delete this->_gameBoard;
}

void TripleTriad::run()
{
	Player *firstPlayer = new Player(this, this->_gameBoard, PIECE_BLUE, PIECE_RED);
	Player *secondPlayer = new Player(this, this->_gameBoard, PIECE_RED, PIECE_BLUE);

	while (this->_gameBoard->gameComplete() == false)
	{
		this->_gameBoard->drawBoard(this->_surface);
		SDL_Flip(this->_surface);
		if (this->_gameBoard->getCurrentPiece() == PIECE_BLUE)
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

	int firstPieces = this->_gameBoard->getNumPieces(PIECE_BLUE);
	int secondPieces = this->_gameBoard->getNumPieces(PIECE_RED);
	
	this->_gameBoard->printBoard();
	std::cout << "Final score: Blue: " << firstPieces << "   Red: " << secondPieces << std::endl;

	// Loop until the user exits.
	this->_gameBoard->drawBoard(this->_surface);
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
						Move *move = this->_gameBoard->getMove((event.button.y - 121) / 101, (event.button.x - 110) / 101, this->_cardChosen);
						if (move != NULL)
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

	TripleTriad *game = new TripleTriad();
	game->run();

	return 0;
}
