#ifndef TRIPLETRIAD_H
#define TRIPLETRIAD_H

#include "SDL.h"

class GameBoard;

class TripleTriad
{
	public:
		// Constructor
		TripleTriad();
		~TripleTriad();

		// Method to run the game.
		void run();

		// Method to check SDL events.
		bool checkEvent(bool moveHuman);

	private:
		// SDL Surface
		SDL_Surface *_surface;

		// Game board.
		GameBoard *_gameBoard;
		
		// Input data
		int _cardChosen;
};

#endif
