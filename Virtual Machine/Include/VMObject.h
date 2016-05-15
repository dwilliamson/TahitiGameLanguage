
#ifndef	_INCLUDED_VMOBJECT_H
#define	_INCLUDED_VMOBJECT_H


class VMObject
{
public:
	// Constructor/Destructor
	VMObject(void);
	~VMObject(void);

	// --- Initialisation -------------------------------------------------

	// Set the class that this object is an instance of
	void	SetClass(VMClass *class_ptr);

	// Set the data segment for this object
	void	SetDataSegment(char *data_segment, int limit);

	// Set the virtual function table for this object
	void	SetFunctionTable(int *function_table, int nb_functions);

	// Set the virtual state table for this object
	void	SetStateTable(int *state_table, int nb_states);

	// Set the import table for this object
	void	SetImportTable(int *import_table, int nb_imports);

	// Set the thread which this object controls
	void	SetThread(VMThread *thread_ptr);

	// Create the native object
	void	SetNative(NativeClass *native);

	// --------------------------------------------------------------------

	// Get the objects data segment
	char	*GetDataSegment(void);

	// Get the size of the object data segment
	int		GetDataSize(void);

	// Get the number of functions in the object virtual function table
	int		GetNbFunctions(void);

	// Get the number of states in the object virtual state table
	int		GetNbStates(void);

	// Get the C++ object
	void	*GetNativeObject(void);

	// Links for linked list
	VMObject	*prev, *next;

private:
	// Class this object is an instance of
	VMClass	*m_Class;

	// Thread this object controls
	VMThread	*m_Thread;

	// Pointer to the data segment
	char	*m_DataSegment;

	// Size of the data segment
	int		m_DataSize;

	// Virtual function table
	int		*m_FunctionTable;

	// Virtual state table
	int		*m_StateTable;

	// Virtual import table
	int		*m_ImportTable;

	// Number of functions
	int		m_NbFunctions;

	// Number of states
	int		m_NbStates;

	// Number of imported functions
	int		m_NbImports;

	// Pointer to the C++ version of this object
	void	*m_NativeObject;

	// Info about the C++ version
	NativeClass	*m_NativeClass;
};


#endif	/* _INCLUDED_VMOBJECT_H */