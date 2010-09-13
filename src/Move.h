#ifndef MOVE_H
#define MOVE_H

#include <iostream>

class Card;

class Move
{
	public:
		Move(int row, int col, Card *card)
		{
			this->row = row;
			this->col = col;
			this->card = card;
		}

		// Stream insertion operator for printing this move.
		friend std::ostream& operator<<(std::ostream &stream, const Move &move);

		// The row and column for this move.
		int row;
		int col;
		
		// The card of the move.
		Card *card;
};

#endif
