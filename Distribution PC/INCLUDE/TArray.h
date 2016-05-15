
#ifndef	_INCLUDED_TARRAY_H
#define	_INCLUDED_TARRAY_H


template <class T, int i = 20>
class TArray
{
public:
	// Constructor
	TArray(void)
	{
		// Allocate space for the elements
		if ((elements = new T[i]) == NULL)
			throw CError("Couldn't allocate TArray");

		position = 0;
		nb_elements = i;
	}

	// Destructor
	~TArray(void)
	{
		// Release the memory
		delete elements;
	}

	int	Grow(int size)
	{
		T	*elptr;

		// Allocate the new space
		if ((elptr = new T[size]) == NULL)
			return (0);

		// Copy the old array onto the new one
		memcpy(elptr, elements, nb_elements * sizeof(T));

		// Set the new size
		nb_elements = size;

		// Swap the old with the new
		delete elements;
		elements = elptr;

		return (1);
	}

	int	Add(T &other)
	{
		// If the array is too small, grow it to 1.8 times its current size
		if (position >= nb_elements)
			if (!Grow((nb_elements * 9) / 5))
				return (0);

		// Assign the element and move on
		elements[position++] = other;

		return (1);
	}

	int AddBlock(T *ptr, int size)
	{
		// If the array is too small, grow it to adjust
		if (position + size >= nb_elements)
			if (!Grow(nb_elements + size))
				return (0);

		// Copy the data
		memcpy(&elements[position], ptr, size * sizeof(T));

		// Move on
		position += size;

		return (1);
	}

	void	Reset(void)
	{
		// Just set the position to 0
		position = 0;
	}

	int	Flush(void)
	{
		// Delete the allocated memory
		delete elements;

		// Allocate the original size
		if ((elements = new T[i]) == NULL)
			return (0);

		position = 0;
		nb_elements = i;

		return (1);
	}

	int GetSize(void)
	{
		return (nb_elements);
	}

	int GetPosition(void)
	{
		return (position);
	}

	T* GetDataPtr(void)
	{
		return (elements);
	}

	// Operator for referencing values like array(i)
	T& operator () (int i)
	{
		// Check for out of bounds
		if (i >= nb_elements)
			throw CError("Out of bounds array reference");

		return(elements[i]);
	}

private:
	// Current add position
	int		position;

	// Actual array
	T		*elements;

	// Number of elements in the array
	int		nb_elements;

};


#endif	/* _INCLUDED_TARRAY_H */