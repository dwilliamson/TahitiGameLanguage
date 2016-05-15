
#ifndef	_INCLUDED_VMFUNCTION_H
#define	_INCLUDED_VMFUNCTION_H


class VMFunction
{
public:
	// Constructor/Destructor
	VMFunction(CFile *file);
	~VMFunction(void);

	// Pointers for the linked list
	VMFunction	*next, *prev;

private:
	// Name of the function
	char	*m_Name;

	// Index within the class
	int		m_Index;

	// Location within the code
	int		m_Location;

	// Return type ID
	int		m_RetTypeID;

	// Return type name
	char	*m_RetTypeName;

	// Is the returned type an array?
	int		m_RetArray;

	// Number of elements in the return array
	int		m_RetElements;

	// Number of parameters passed to function
	int		m_NbParams;
};


#endif	/* _INCLUDED_VMFUNCTION_H */