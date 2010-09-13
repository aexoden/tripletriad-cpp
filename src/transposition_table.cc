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
