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

int Card::_next_id = 0;

Card::Card(int top, int bottom, int left, int right, Element element) :
	top(top),
	bottom(bottom),
	left(left),
	right(right),
	element(element),
	id(Card::_next_id++)
{ }

void Card::render(SDL_Surface * surface, int x, int y) const
{
	char values[4];

	values[0] = (this->top    == 10) ? 'A' : this->top    + 48;
	values[1] = (this->bottom == 10) ? 'A' : this->bottom + 48;
	values[2] = (this->left   == 10) ? 'A' : this->left   + 48;
	values[3] = (this->right  == 10) ? 'A' : this->right  + 48;
				
	characterRGBA(surface, x + 40, y + 5, values[0], 255, 255, 255, 255);
	characterRGBA(surface, x + 40, y + 25, values[1], 255, 255, 255, 255);
	characterRGBA(surface, x + 35, y + 15, values[2], 255, 255, 255, 255);
	characterRGBA(surface, x + 45, y + 15, values[3], 255, 255, 255, 255);
				
	switch(this->element)
	{
		case ELEMENT_NONE:
			break;

		case ELEMENT_FIRE:
			stringRGBA(surface, x + 5, y + 38, "Fire", 255, 255, 255, 255);
			break;

		case ELEMENT_ICE:
			stringRGBA(surface, x + 5, y + 38, "Ice", 255, 255, 255, 255);
			break;

		case ELEMENT_THUNDER:
			stringRGBA(surface, x + 5, y + 38, "Thunder", 255, 255, 255, 255);
			break;

		case ELEMENT_POISON:
			stringRGBA(surface, x + 5, y + 38, "Poison", 255, 255, 255, 255);
			break;

		case ELEMENT_EARTH:
			stringRGBA(surface, x + 5, y + 38, "Earth", 255, 255, 255, 255);
			break;

		case ELEMENT_WIND:
			stringRGBA(surface, x + 5, y + 38, "Wind", 255, 255, 255, 255);
			break;

		case ELEMENT_WATER:
			stringRGBA(surface, x + 5, y + 38, "Water", 255, 255, 255, 255);
			break;

		case ELEMENT_HOLY:
			stringRGBA(surface, x + 5, y + 38, "Holy", 255, 255, 255, 255);
			break;
	}
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

		case ELEMENT_ICE:
			stream << "Ice";
			break;

		case ELEMENT_THUNDER:
			stream << "Thunder";
			break;

		case ELEMENT_POISON:
			stream << "Poison";
			break;

		case ELEMENT_EARTH:
			stream << "Earth";
			break;

		case ELEMENT_WIND:
			stream << "Wind";
			break;

		case ELEMENT_WATER:
			stream << "Water";
			break;

		case ELEMENT_HOLY:
			stream << "Holy";
			break;
	}

	stream << "}";

	return stream;
}
