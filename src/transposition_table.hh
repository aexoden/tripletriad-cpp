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

#ifndef TRIPLETRIAD_TRANSPOSITION_TABLE_HH
#define TRIPLETRIAD_TRANSPOSITION_TABLE_HH

class Move;

class TranspositionTable
{
	public:
		// A public struct representing the entries in the table. It's public so that
		// external classes can directly access the entries without needing several
		// get and set methods (for optimization purposes).
		struct Entry
		{
			unsigned long long key;
			int lowerBound;
			int upperBound;
			int ply;
			Move *bestMove;
		};

		// Retrieve an entry from the table.
		Entry* const getEntry(unsigned long long key);

		// Reset a particular entry, and return the entry.
		Entry* const newEntry(unsigned long long key);

		// Reset the table.
		void reset();

	private:
		// The size of the table.
		static const int TABLE_SIZE = 1048575;

		// The actual array that holds the table.
		Entry _table[TABLE_SIZE];
};

#endif
