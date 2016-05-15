
#ifndef	_INCLUDED_THASH_H
#define	_INCLUDED_THASH_H


#define	MAX_HASH_LABEL		64


class CHashCell
{
public:
	CHashCell(void) : string(NULL) { }

	~CHashCell(void)
	{
		// Release string memory
		if (string)
		{
			delete [] string;
			string = NULL;
		}
	}

	void SetName(char *name)
	{
		// If the pointers are the same, no need to set the name
		if (name == string)
			return;

		// Release if the name is already set
		if (string)
			delete [] string;

		// Allocate the new string
		if ((string = new char[strlen(name) + 1]) == NULL)
			throw CError("Couldn't allocate hash cell name");

		// Copy it
		strcpy(string, name);
	}

	// Return the hash cell name
	char *GetName(void) { return (string); }

	// Pointer to next cell
	CHashCell	*next;

private:
	// String for comparisons
	char		*string;
};


template <int i = 211>
class THash
{
public:
	THash(void)
	{
		// Allocate memory
		if ((entries = new CHashCell *[i]) == NULL)
			throw CError("Couldn't allocate the hash table with %d elements", i);

		memset(entries, 0, i * sizeof(CHashCell *));
		nb_entries = i;
		enum_nb = 0;
		enum_cell = NULL;
		nb_added = 0;
	}

	~THash(void)
	{
		delete [] entries;
	}

	void Add(const char *string, CHashCell *entry)
	{
		unsigned int	index;

		// Setup the entry
		entry->next = NULL;
		entry->SetName((char *)string);

		// Get the table index and wrap it
		index = ElfHash(string) % nb_entries;

		if (entries[index] == NULL)
		{
			// Just add to the beginning of the list
			entries[index] = entry;
		}
		else
		{
			CHashCell	*cur;
			
			// Search for the end of the linked list
			for (cur = entries[index]; cur->next; cur = cur->next)
				;

			// Add it
			cur->next = entry;
		}

		nb_added++;
	}

	CHashCell* GetEntry(const char *string)
	{
		unsigned int	index;
		CHashCell		*cur;

		// Get the table index and wrap it
		index = ElfHash(string) % nb_entries;

		// No entries at this location
		if (entries[index] == NULL)
			return (NULL);

		// Loop through each entry in the list
		for (cur = entries[index]; cur; cur = cur->next)
		{
			// Found a match?
			if (!strcmp(string, cur->GetName()))
				return (cur);
		}

		return (NULL);
	}

	void	Remove(CHashCell *cell_ptr)
	{
		unsigned int	index;
		CHashCell		*cur, *prev = NULL;

		// Get the table index and wrap it
		index = ElfHash(cell_ptr->GetName()) % nb_entries;

		// No entries
		if (entries[index] == NULL)
			return;

		// For each entry
		for (cur = entries[index]; cur; cur = cur->next)
		{
			// Found a match?
			if (!strcmp(cell_ptr->GetName(), cur->GetName()))
			{
				// Remove it!
				if (prev) prev->next = cell_ptr->next;
				else entries[index] = NULL;

				nb_added--;

				return;
			}

			prev = cur;
		}
	}

	CHashCell* Enumerate(void)
	{
		int			x;
		CHashCell	*cell_ptr;

		cell_ptr = enum_cell;

		// For every entry
		for (x = enum_nb; x < nb_entries; x++)
		{
			// If there is no designated cell pointer, point it to the first in this entry
			if (cell_ptr == NULL)
				cell_ptr = entries[x];

			if (cell_ptr == NULL)
				continue;

			// Point to the next cell for the next enumeration call
			enum_cell = cell_ptr->next;
			if (enum_cell == NULL) x++;
			enum_nb = x;

			return (cell_ptr);
		}

		// Reset
		enum_nb = 0;
		enum_cell = NULL;

		return (NULL);
	}

	int	GetNumAddedItems(void)
	{
		return (nb_added);
	}

private:
	unsigned int	ElfHash(const char *string)
	{
		unsigned int	h = 0, g;

		// Generate a unique index
		while (*string)
		{
			h = (h << 4) + *string++;

			if (g = h & 0xF0000000)
				h ^= g >> 24;

			h &= ~g;
		}

		return (h);
	}

	// Number of items that have been added to the hash table
	int		nb_added;

	// Number of entries in the hash table
	int		nb_entries;

	// List of entries
	CHashCell	**entries;

	// Enumeration variables
	int			enum_nb;
	CHashCell	*enum_cell;
};


#endif