#ifndef SQUARE_H
#define SQUARE_H

#include <iostream>

#include "enum.h"

class Square
{
	public:
		Square()
		{
			this->elementalBonus = 0;
			this->row = 0;
			this->col = 0;
		}
		
		Element element;
		Piece owner;
		int elementalBonus;
		int row;
		int col;
		
		Card* getCard()
		{
			return this->card;
		}
		
		void setCard(Card *card)
		{
			this->card = card;
			this->calculateElementalBonus();
		}
		
	private:
		Card *card;
		
		void calculateElementalBonus()
		{
			if ((this->element != ELEMENT_NONE) && (this->card != NULL) && (this->card->element != ELEMENT_NONE))
			{
				if (this->element == this->card->element)
				{
					this->elementalBonus = 1;
				}
				else
				{
					this->elementalBonus = -1;
				}
			}
		}
};

#endif
