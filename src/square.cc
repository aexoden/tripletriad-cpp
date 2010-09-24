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

#include "card.hh"
#include "square.hh"

int Square::_next_id = 0;

Square::Square(int row, int col, Element element) :
	row(row),
	col(col),
	element(element),
	id(Square::_next_id++),
	_neighbors(4)
{ }

std::shared_ptr<Square> Square::get_neighbor(Direction direction) const
{
	return this->_neighbors[direction];
}

std::vector<std::shared_ptr<Square>> Square::build_squares(int rows, int cols)
{
	std::vector<std::shared_ptr<Square>> squares(rows * cols);

	for (int row = 0; row < rows; row++)
		for (int col = 0; col < cols; col++)
			squares[row * cols + col] = std::make_shared<Square>(Square(row, col, ELEMENT_NONE));

	for (int row = 0; row < rows; row++)
	{
		for (int col = 0; col < cols; col++)
		{
			if (col > 0)
				squares[row * cols + col]->_neighbors[WEST] = squares[row * cols + col - 1];

			if (row > 0)
				squares[row * cols + col]->_neighbors[NORTH] = squares[(row - 1) * cols + col];

			if (col < cols - 1)
				squares[row * cols + col]->_neighbors[EAST] = squares[row * cols + col + 1];

			if (row < rows - 1)
				squares[row * cols + col]->_neighbors[SOUTH] = squares[(row + 1) * cols + col];
		}
	}

	return squares;
}

std::ostream & operator<<(std::ostream & stream, const Square & square)
{
	std::ostringstream oss;
	oss << "(" << square.row << ", " << square.col << ")";

	return stream << oss.str();
}
