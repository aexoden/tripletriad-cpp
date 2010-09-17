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

#include "SDL_gfxPrimitives.h"

#include "card.hh"

Card::Card(Piece owner, int top, int bottom, int left, int right, Element element) :
	top(top),
	bottom(bottom),
	left(left),
	right(right),
	element(element),
	_owner(owner),
	_square()
{ }

void Card::render(SDL_Surface * surface, int col, int row) const
{
	char values[4];

	values[0] = (this->top    == 10) ? 'A' : this->top    + 48;
	values[1] = (this->bottom == 10) ? 'A' : this->bottom + 48;
	values[2] = (this->left   == 10) ? 'A' : this->left   + 48;
	values[3] = (this->right  == 10) ? 'A' : this->right  + 48;
				
	characterRGBA(surface, col + 40, row + 5, values[0], 255, 255, 255, 255);
	characterRGBA(surface, col + 40, row + 25, values[1], 255, 255, 255, 255);
	characterRGBA(surface, col + 35, row + 15, values[2], 255, 255, 255, 255);
	characterRGBA(surface, col + 45, row + 15, values[3], 255, 255, 255, 255);
				
	switch(this->element)
	{
		case ELEMENT_NONE:
			break;

		case ELEMENT_FIRE:
			stringRGBA(surface, col + 5, row + 38, "Fire", 255, 255, 255, 255);
			break;

		default:
			break;
	}
}

Piece Card::get_owner() const
{
	return this->_owner;
}

void Card::set_owner(Piece owner)
{
	this->_owner = owner;
}

std::shared_ptr<Square> Card::get_square() const
{
	return this->_square;
}

void Card::set_square(std::shared_ptr<Square> square)
{
	this->_square = square;
}

std::ostream & operator<<(std::ostream & stream, const Card & card)
{
	char values[4];

	values[0] = (card.top    == 10) ? 'A' : card.top    + 48;
	values[1] = (card.bottom == 10) ? 'A' : card.bottom + 48;
	values[2] = (card.left   == 10) ? 'A' : card.left   + 48;
	values[3] = (card.right  == 10) ? 'A' : card.right  + 48;

	stream << "{" << values[0] << ", " << values[1] << ", " << values[2] << ", " << values[3] << ", ";

	switch (card.element)
	{
		case ELEMENT_NONE:
			stream << "None";
			break;

		case ELEMENT_FIRE:
			stream << "Fire";
			break;

		default:
			stream << "Unknown";
			break;
	}

	stream << "}";

	return stream;
}
