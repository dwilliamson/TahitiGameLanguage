
#ifndef	_INCLUDED_CLASS_H
#define	_INCLUDED_CLASS_H


#define CLASS_FLAGS_ABSTRACT		0x0001
#define CLASS_FLAGS_INTERFACE		0x0002


class Class : public CHashCell
{
public:
	// Constructor/Destructor
	Class(void);
	~Class(void);

	// Setup inheritance
	void		SetSuperClass(char *name);

	// Get the name of the super-class
	char		*GetSuperClass(void);

	// Does this class inherit remotely from...
	int			HasSuperClass(char *name);

	// Mark this class for compilation
	void		SetForCompile(char *file);

	// Does this class need compiling?
	int			NeedsCompile(void);

	// Get the class filename
	char		*GetFilename(void);

	// Set the object file this class compiles to
	void		SetObjectFile(char *file);

	// Get the object file this class compiles to
	char		*GetObjectFile(void);

	// Create a new address within the class
	int			NewAddress(int size, int align);

	// Get the current address pointer
	int			GetCurAddress(void);

	// Retrieve a new class-specific function ID
	int			NewFunctionIndex(void);

	// Retrieve a new class-specific state ID
	int			NewStateIndex(void);

	// Retrieve a new class-specific import ID
	int			NewImportIndex(void);

	// Does this class inherit?
	int			DoesInherit(void);

	// Tell the class it has been defined
	void		SetDefined(void);

	// Is the class defined?
	int			IsDefined(void);

	// Set a flag in the class
	void		SetFlag(int flag);

	// Get the class flags
	int			GetFlags(void);

	// Set the entry point function
	void		SetEntryPoint(int index);

	// Get the entry point function
	int			GetEntryPoint(void);

	// Retrieve a variable somewhere within the scope of the class
	Variable	*GetVariable(char *name, int flags);

	// Retrieve a function somewhere within the scope of the class
	Function	*GetFunction(char *name, int flags);

	// Retrieve an imported function somewhere within the scope of the class
	Function	*GetImport(char *name);

	// Retrieve a string literal somewhere within the scope of the class
	Variable	*GetStringLiteral(char *string);

	// Shift function indices above their super-class
	void		ShiftFunctionIndices(int value);

	// Shift import function indices above their super-class
	void		ShiftImportIndices(int value);

	// Shift state indices above their super-class
	void		ShiftStateIndices(int value);

	// Shift variable addresses above their super-class
	void		ShiftVariableAddresses(int value);

	// Try and validate update all entry points
	void		ManageEntryPoint(int value);

	// Get the last function declaration within the class hierarchy
	Function	*GetLastFuncDecl(char *state, char *name);

	// Resolve function polymorphism issues
	void		ResolveFuncOverwrites(void);

	// Get the last imported function declaration within the class hierarchy
	Function	*GetLastImpDecl(char *name);

	// Resolve imported function polymorphism issues
	void		ResolveImpOverwrites(void);

	// Get the last state declaration within the class hierarchy
	State		*GetLastStateDecl(char *state);

	// Resolve state polymorphism issues
	void		ResolveStateOverwrites(void);

	// Begin building virtual call tables
	void		BuildVTable(Class *parent);

	// Begin building import tables
	void		BuildITable(Class *parent);

	// Begin building state tables
	void		BuildSTable(Class *parent);

	// Correct all addresses as they are in the code segment
	void		UpdateCodeAddresses(int position);

	// Get a state within the class by name
	State		*GetState(char *name);

	// Class member variables
	THash<>		variables;

	// List of all states in this class
	THash<>		states;

	// List of all functions in this class
	THash<>		functions;

	// List of all imported functions in this class
	THash<>		imp_functions;

	// List of string literals defined in the class
	THash<>		string_literals;

	// List of classes which inherit from this one
	TArray<Class *, 5>	children;

	// Virtual function table
	TArray<Function *>	vtable;

	// Function import table
	TArray<Function *>	itable;

	// State table
	TArray<State *>		stable;

	// Current class state
	State		*cur_state;

	// Currently active function (cannot be nested)
	Function	*cur_function;

	// Pointer to the super-class
	Class		*sclass_ptr;

	// Number of entries in the function vtable
	int			nb_vt_entries;

	// Number of entries in the import table
	int			nb_it_entries;

	// Number of entries in the state table
	int			nb_st_entries;

	// Number of functions in the class (only valid at link stage)
	int			nb_functions;

	// Number of imported functions in the class (only valid at link stage)
	int			nb_impfuncs;

	// Number of states in the class (only valid at link stage)
	int			nb_states;

	// Number of variables in the class (only valid at link stage)
	int			nb_variables;

	// Write class information to file
	void		Write(CFile *file);

	// Read class information from file
	void		Read(CFile *file);

	// Write information tailored for a VMU file
	void		WriteVMUInfo(CFile *file);

	// Write the class data segment
	void		WriteDataSegment(char *data);

private:
	// Filename of VMC file
	char		filename[PATH_SIZE];

	// Filename of VMO file
	char		object_file[PATH_SIZE];

	// Does this class need to be compiled?
	int			need_compile;

	// Temp function to write a dword to file
	void		WriteValue(CFile *file, int value, int where);

	// Has this class been defined yet (it's a shell otherwise)
	int			defined;

	// Current free variable address
	int			cur_address;

	// Current free function number
	int			cur_funcnum;

	// Current free state number
	int			cur_statenum;

	// Current free import number
	int			cur_impnum;

	// Does class inherit?
	int			inherits;

	// Name of the class that this inherits from
	char		super_class[MAX_LABEL_SIZE];

	// Flags describing properties of the class
	int			flags;

	// VM entry point function index (always the constructor)
	int			entry_point;
};


#endif	/* _INCLUDED_CLASS_H */