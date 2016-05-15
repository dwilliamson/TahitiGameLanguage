
#ifndef	_INCLUDED_CLASS_H
#define	_INCLUDED_CLASS_H


#define CLASS_FLAGS_ABSTRACT		0x0001


class Class : public CHashCell, public TreeViewEntry
{
public:
	// Constructor/Destructor
	Class(CFile *file);
	virtual ~Class(void);

	// Add this class to the application tree view
	void	AddToTreeView(int level);

	// Print information about this treeview entry
	void	PrintInfo(void);

	// Class name
	char	m_Name[256];

	// Does this class inherit?
	int		m_Inherits;

	// Name of the super-class
	char	m_SuperClass[256];

	// Number of variables in the class
	int		m_NbVariables;

	// Number of functions in the class
	int		m_NbFunctions;

	// Number of imported functions
	int		m_NbImports;

	// Number of states in the class
	int		m_NbStates;

	// List of variables in the class
	THash<>	m_VariableList;

	// List of functions in the class
	THash<>	m_FunctionList;

	// List of imported functions in the class
	TArray<Function *>	m_ImportList;

	// List of states in the class
	THash<>	m_StateList;

	// Class flags
	int		m_Flags;

	// Code entry point
	int		m_EntryPoint;

	// List of classes that derive from this one
	TArray<Class *>		m_Children;

	// Pointer to the super-class
	Class	*m_SClassPtr;

	// Virtual function table
	Table	*m_VTable;

	// Virtual state table
	Table	*m_STable;
};


#endif	/* _INCLUDED_CLASS_H */