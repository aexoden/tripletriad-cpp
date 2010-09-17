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

#include <limits>
#include <string.h>

#include "transposition_table.hh"

TranspositionTable::Entry* const TranspositionTable::getEntry(unsigned long long key)
{
	Entry *entry = &(this->_table[key % TABLE_SIZE]);

	if (entry->key == key)
	{
		return entry;
	}

	return NULL;
}

TranspositionTable::Entry* const TranspositionTable::newEntry(unsigned long long key)
{
	Entry *entry = &(this->_table[key % TABLE_SIZE]);

	entry->key = key;
	entry->lowerBound = std::numeric_limits<int>::min();
	entry->upperBound = std::numeric_limits<int>::max();
	entry->ply = -1;
	entry->bestMove = NULL;

	return entry;
}

void TranspositionTable::reset()
{
	memset(this->_table, 0, TABLE_SIZE * sizeof(Entry));
}
