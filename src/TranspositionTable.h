#ifndef TRANSPOSITIONTABLE_H
#define TRANSPOSITIONTABLE_H

#include "Move.h"

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
