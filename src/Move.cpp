#include "Card.h"
#include "Move.h"

std::ostream& operator<<(std::ostream &stream, const Move &move)
{
	// Generate the element string.
	char elementString[10];
	switch (move.card->element)
	{
		case ELEMENT_NONE: strcpy(elementString, "None"); break;
		case ELEMENT_FIRE: strcpy(elementString, "Fire"); break;
		default: strcpy(elementString, "Unknown"); break;
	}
	
	// Generate the attack value characters
	char attacks[4];
	attacks[0] = (move.card->top    == 10) ? 'A' : move.card->top    + 48;
	attacks[1] = (move.card->bottom == 10) ? 'A' : move.card->bottom + 48;
	attacks[2] = (move.card->left   == 10) ? 'A' : move.card->left   + 48;
	attacks[3] = (move.card->right  == 10) ? 'A' : move.card->right  + 48;

	char moveString[30];
	sprintf(moveString, "{%c, %c, %c, %c, %s} at (%d, %d)", attacks[0], attacks[1], attacks[2], attacks[3], elementString, move.row + 1, move.col + 1);
	stream << moveString;

	return stream;
}