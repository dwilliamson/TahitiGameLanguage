
#ifndef	_INCLUDED_TSTACK_H
#define	_INCLUDED_TSTACK_H


template <class T, int i = 100>
class TStack
{
public:
	// Constructor
	TStack(void)
	{
		nb_elements = i;
		position = 0;
	}

	void	Push(T value)
	{
		// Return if not enough room
		if (position == nb_elements)
			throw CError("Stack overflow");

		// Insert
		elements[position++] = value;
	}

	T	Pop(void)
	{
		// Return if underflow iminent
		if (position == 0)
			throw CError("Stack underflow");

		// Take out
		return (elements[--position]);
	}

	// Top(0) is the top of the stack
	T	Top(int where)
	{
		// Return if out of bounds
		if (where < 0 || where >= position)
			throw CError("Out of bounds stack reference");

		return (elements[where]);
	}

	void	Flush(void)
	{
		// Set back to the beginning
		position = 0;
	}

	int	IsFull(void)
	{
		return (position >= nb_elements);
	}

	int IsEmpty(void)
	{
		return (position <= 0);
	}

private:
	// Where everything is stored
	T		elements[i];

	// Number of elemnts in the stack
	int		nb_elements;

	// Position within the stack
	int		position;
};


#endif	/* _INCLUDED_TSTACK_H */