
#ifndef	_INCLUDED_ENVIRONMENT_H
#define	_INCLUDED_ENVIRONMENT_H


class Environment
{
public:
	// Constructor/Destructor
	Environment(void);
	~Environment(void);

	// Add a pre-built object to the environment
	void		AddObject(const char *filename);

	// Perform the 1st pass on the VMC file
	void		ConstructClass(const char *filename);

	// Compile all classes in the environment that need to be
	void		CompileClasses(void);

	// Add a created class to the list of classes
	void		AddClassPtr(Class *class_ptr);

	// Take all classes in the environment and link them
	void		LinkClasses(void);

	// Build all class-specific information that needs to be done for output
	void		PrepareForOutput(void);

	// Write out the final unit
	void		GenerateVMUnit(const char *output);

	// Get a class from the environment by name
	Class		*GetClass(char *name);

	// Get the currently active class in the environment
	Class		*GetActiveClass(void);

	// Set the currently active class in the environment
	void		SetActiveClass(Class *class_ptr);

	// Add an IDL unit to the environment to import all the native interfaces
	void		AddIDL(const char *filename);

private:
	void		CompileClassPtr(Class *class_ptr);

	// Update state inheritance information
	void		FinaliseStates(void);

	// Add the object code to the VMU
	void		AddObjectToVMU(Class *class_ptr);

	// Add the class information to the VMU
	void		AddClassToVMU(Class *class_ptr);

	// Perform the 2nd pass on the VMC file
	void		CompileClass(const char *filename, const char *output);

	// Print information about the environment to file
	void		Debug(void);

	// All the classes within the current environment
	THash<>		class_hash;

	// Array of base classes in the environment
	TArray<Class *>	base_classes;

	// Current tokeniser
	Tokeniser		*tokeniser;

	// Current code generator
	CodeGenerator	*code_gen;

	// Current IDL parser
	IDLParser		*idl_parse;

	// Current file under scrutiny
	const char	*cur_filename;

	// Currently active class in the environment
	Class		*cur_class;

	// Global file pointer
	CFile		*file;

	// VM Unit to compile to
	VMUFile		*unit;
};


extern	VMOFile		*g_Object;
extern	VMUFile		*g_Unit;
extern	Environment	*g_Env;


#endif	/* _INCLUDED_ENVIRONMENT_H */