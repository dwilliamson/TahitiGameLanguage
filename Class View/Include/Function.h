
#ifndef	_INCLUDED_FUNCTION_H
#define	_INCLUDED_FUNCTION_H


class Function : public CHashCell, public TreeViewEntry
{
public:
	// Constructor/Destructor
	Function(CFile *file);
	virtual ~Function(void);

	// Print information about this treeview entry
	void	PrintInfo(void);

	// Function name
	char	m_Name[256];

	// Index within the class
	int		m_Index;

	// Location of the function in code
	int		m_Location;

	// Return type ID
	int		m_RetTypeID;

	// Return type name if it's custom
	char	m_RetTypeName[256];

	// If the return type is an array
	int		m_RetArray;

	// Number of elements in the return type array
	int		m_RetElements;

	// Number of passed parameters
	int		m_NbParams;

	// Is this an imported function?
	int		m_Import;

	// List of passed parameters
	TArray<Variable *>	m_ParamList;
};


#endif	/* _INCLUDED_FUNCTION_H */