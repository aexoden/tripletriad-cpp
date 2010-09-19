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

#ifndef TRIPLETRIAD_SQUARE_HH
#define TRIPLETRIAD_SQUARE_HH

#include <memory>
#include <vector>

#include "common.hh"

enum Direction
{
	NORTH,
	SOUTH,
	EAST,
	WEST
};

class Card;

class Square
{
	public:
		Square(int row, int col, Element element);

		std::shared_ptr<Card> get_card() const;
		void set_card(std::shared_ptr<Card> card);

		int get_elemental_adjustment() const;

		const std::shared_ptr<const Square> get_neighbor(Direction direction);

		static std::vector<std::shared_ptr<Square>> build_squares(int rows, int cols);
		
		const int row, col;
		Element element;

	private:
		std::shared_ptr<Card> _card;

		std::vector<std::shared_ptr<Square>> _neighbors;
};

#endif
