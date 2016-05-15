
/*
 * TLinkedList
 * -----------
 * Linked list template class. Any classes that become elements of the list must have its
 * own defined "prev" and "next" member pointers.
 *
 * Usage is like...
 *
 * TLinkedList<object>	list;
 * 
 * list.AddLast(object_ptr);
 *
 * for (object_ptr = list.GetFirst; object_ptr; object_ptr = object_ptr->next)
 *    do_stuff;
 *
 */

#ifndef	_INCLUDED_TLINKEDLIST_H
#define	_INCLUDED_TLINKEDLIST_H


template <class T>
class TLinkedList
{
public:
	// Constructor
	TLinkedList(void)
	{
		// Nothing in the list as of yet
		first = NULL;
		last = NULL;
		nb_entries = 0;
	}

	void	AddLast(T *element)
	{
		// Anything in the list?
		if (first == NULL)
		{
			// Make it the first
			element->next = NULL;
			element->prev = NULL;
			first = element;
			last = element;
		}
		else
		{
			// Add to the end
			element->next = NULL;
			element->prev = last;
			last->next = element;
			last = element;
		}

		nb_entries++;
	}

	void	AddFirst(T *element)
	{
		// Anything in the list?
		if (first == NULL)
		{
			// Make it the first
			element->next = NULL;
			element->prev = NULL;
			first = element;
			last = element;
		}
		else
		{
			// Add to the beginning
			element->next = first;
			element->prev = NULL;
			first->prev = element;
			first = element;
		}

		nb_entries++;
	}

	void	Remove(T *element)
	{
		// Remove if valid
		if (element)
		{
			if (element->next) element->next->prev = element->prev;
			if (element->prev) element->prev->next = element->next;
			if (element == first) first = element->next;
			if (element == last) last = element->prev;

			nb_entries--;
		}
	}

	void	Flush(void)
	{
		T	*element, *next;

		for (element = first; element; element = next)
		{
			// Backup the next element
			next = element->next;

			// De-link the current element
			element->next = NULL;
			element->prev = NULL;
		}

		// Empty!
		first = NULL;
		last = NULL;
	}

	// Functions to grab either end of the list
	T	*GetFirst(void) { return (first);	}
	T	*GetLast(void)	{ return (last);	}

	// Get the number of entries in the linked list
	int	GetEntries(void) { return (nb_entries); }

private:
	// First and last elements in the linked list
	T		*first;
	T		*last;

	// Number of entries in the linked list
	int		nb_entries;
};


#endif	/* _INCLUDED_TLINKEDLIST_H */