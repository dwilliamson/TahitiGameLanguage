
#ifndef	_INCLUDED_VMIMPORT_H
#define	_INCLUDED_VMIMPORT_H


class VMImport
{
public:
	// Constructor/Destructor
	VMImport(CFile *file);
	~VMImport(void);

	// Get the name of the imported function
	char	*GetName(void);

	// Get the number of passed parameters
	int		GetNbParams(void);

	// Get the type the function returns
	int		GetReturnType(void);

	// Links for the linked list
	VMImport	*prev, *next;

private:
	// Function import name
	char	*m_Name;

	// Index within the class
	int		m_Index;

	// Return type ID
	int		m_RetTypeID;

	// Return type name if it's custom
	char	*m_RetTypeName;

	// If the return type is an array
	int		m_RetArray;

	// Number of elements in the return type array
	int		m_RetElements;

	// Number of passed parameters
	int		m_NbParams;

	// List of passed parameters
	TArray<VMVariable *>	m_ParamList;
};


#endif	/* _INCLUDED_VMIMPORT_H */