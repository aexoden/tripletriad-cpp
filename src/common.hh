#ifndef ENUM_H
#define ENUM_H

#define VERSION "0.2.0_pre0"

// Enumerated constants representing the various possible elements.
enum Element
{
	ELEMENT_NONE = 0,
	ELEMENT_FIRE = 1
};

// Enumerated constants representing the pieces on the board.
enum Piece
{
	PIECE_NONE    = 0,
	PIECE_RED     = 1,
	PIECE_BLUE    = 2,
	PIECE_INVALID = 3
};
		
#endif
