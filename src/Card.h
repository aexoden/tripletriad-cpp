#ifndef CARD_H
#define CARD_H

#include <iostream>

#include "SDL.h"
#include "SDL_gfxPrimitives.h"

#include "enum.h"

class Card
{
	public:
		Card(int top, int bottom, int left, int right, Element element)
		{
			this->top = top;
			this->bottom = bottom;
			this->left = left;
			this->right = right;
			this->element = element;
			
			this->ID = Card::nextID;
			Card::nextID++;
		}
		
		// Method for the card to draw itself.
		void draw(SDL_Surface *surface, int colOffset, int rowOffset);

		// Stream insertion operator for printing this move.
		friend std::ostream& operator<<(std::ostream &stream, const Card &card);

		// The attack values of the card.
		int top;
		int bottom;
		int left;
		int right;
		
		// The card's unique ID
		int ID;
		
		// The element of the card.
		Element element;
		
	private:
		// The current index.
		static int nextID;
};

#endif
