#include "Card.h"

int Card::nextID = 0;

void Card::draw(SDL_Surface *surface, int colOffset, int rowOffset)
{
	// Calculate the four attack values.
	char attacks[4];
	attacks[0] = (this->top    == 10) ? 'A' : this->top    + 48;
	attacks[1] = (this->bottom == 10) ? 'A' : this->bottom + 48;
	attacks[2] = (this->left   == 10) ? 'A' : this->left   + 48;
	attacks[3] = (this->right  == 10) ? 'A' : this->right  + 48;
				
	// Actually draw the attack values.
	characterRGBA(surface, colOffset + 40, rowOffset + 5, attacks[0], 255, 255, 255, 255);
	characterRGBA(surface, colOffset + 40, rowOffset + 25, attacks[1], 255, 255, 255, 255);
	characterRGBA(surface, colOffset + 35, rowOffset + 15, attacks[2], 255, 255, 255, 255);
	characterRGBA(surface, colOffset + 45, rowOffset + 15, attacks[3], 255, 255, 255, 255);
				
	// Draw the card element.
	switch(this->element)
	{
		case ELEMENT_NONE: break;
		case ELEMENT_FIRE: stringRGBA(surface, colOffset + 5, rowOffset + 38, "Fire", 255, 255, 255, 255); break;
		default: break;
	}
}

std::ostream& operator<<(std::ostream &stream, const Card &card)
{
	// Generate the element string.
	char elementString[10];
	switch (card.element)
	{
		case ELEMENT_NONE: strcpy(elementString, "None"); break;
		case ELEMENT_FIRE: strcpy(elementString, "Fire"); break;
		default: strcpy(elementString, "Unknown"); break;
	}
	
	// Generate the attack value characters
	char attacks[4];
	attacks[0] = (card.top    == 10) ? 'A' : card.top    + 48;
	attacks[1] = (card.bottom == 10) ? 'A' : card.bottom + 48;
	attacks[2] = (card.left   == 10) ? 'A' : card.left   + 48;
	attacks[3] = (card.right  == 10) ? 'A' : card.right  + 48;

	char cardString[7];
	sprintf(cardString, "{%c, %c, %c, %c, %s}", attacks[0], attacks[1], attacks[2], attacks[3], elementString);
	stream << cardString;

	return stream;
}
