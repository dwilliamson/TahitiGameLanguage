
#ifndef	_INCLUDED_VMCLASS_H
#define	_INCLUDED_VMCLASS_H


#define CLASS_FLAGS_ABSTRACT		0x0001
#define CLASS_FLAGS_INTERFACE		0x0002


class VMClass : CHashCell
{
public:
	// Set the parent
	void	SetUnit(VMUnit *unit_ptr);

	// Get the name of the class
	char	*GetName(void);

	// Get the default class data segment
	char	*GetDataSegment(void);

	// Get the default class function table
	int		*GetFunctionTable(void);

	// Get the default class state table
	int		*GetStateTable(void);

	// Get the default class import table
	int		*GetImportTable(void);

	// Get the size of the class data segment
	int		GetDataSize(void);

	// Get the number of functions in the class virtual function table
	int		GetNbFunctions(void);

	// Get the number of states in the class virtual state table
	int		GetNbStates(void);

	// Get the number of imports in the class import table
	int		GetNbImports(void);

	// Get the class flags
	int		GetFlags(void);

	// Get the program entry point
	int		GetEntryPoint(void);

	// Build the import table for this class
	void	BuildImportTable(void);

	// Get the equivalent native C++ class
	NativeClass	*GetNative(void);

	// Set the equivalent native C++ class
	void	SetNative(NativeClass *class_ptr);

	friend class	VMUnit;

	// Pointers for the linked list
	VMClass		*next, *prev;

private:
	// Private constructor/destructor
	VMClass(CFile *file);
	~VMClass(void);

	// VMUnit parent
	VMUnit		*m_Unit;

	// Name of the class
	char		*m_Name;

	// Name of the super-class
	char		*m_SuperClass;

	// Does this class inherit?
	int			m_Inherits;

	// Class flags
	int			m_Flags;

	// Class entry point
	int			m_EntryPoint;

	// List of class variables
	TLinkedList<VMVariable>	m_VariableList;

	// List of class functions
	TLinkedList<VMFunction>	m_FunctionList;

	// List of import functions
	TLinkedList<VMImport>	m_ImportList;

	// List of class states
	TLinkedList<VMState>	m_StateList;

	// Default data segment for this class
	char		*m_DataSegment;

	// Size of the data segment
	int			m_DataSize;

	// Virtual function table
	int			*m_FunctionTable;

	// Virtual state table
	int			*m_StateTable;

	// Virtual import function table
	int			*m_ImportTable;

	// Total number of functions in the class
	int			m_NbFunctions;

	// Number of states in the class
	int			m_NbStates;

	// Number of imported functions in the class
	int			m_NbImports;

	// Native C++ class pointer, if there is one
	NativeClass	*m_Native;

	// List of indexes for each method, into
	int			*m_MethodIndex;
};


#endif	/* _INCLUDED_VMCLASS_H */