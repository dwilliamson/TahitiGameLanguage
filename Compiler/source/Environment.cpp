
#include "..\include\main.h"


VMOFile		*g_Object = NULL;
VMUFile		*g_Unit = NULL;
Environment	*g_Env = NULL;


Environment::Environment(void)
{
	g_Env = this;
	cur_class = NULL;
}


Environment::~Environment(void)
{
}


void Environment::AddClassPtr(Class *class_ptr)
{
	infunc(Environment::AddClassPtr);

	Class	*tclass;

	// Check to see if the class is already in the environment
	if ((tclass = (Class *)class_hash.GetEntry(class_ptr->GetName())) == NULL)
	{
		// Add to the classes in the environment
		class_hash.Add(class_ptr->GetName(), class_ptr);
	}
	else
	{
		// Throw an error if the class has already been defined
		if (tclass->IsDefined())
			throw CompileError("(File %s) Second definition of class %s found", cur_filename, tclass->GetName());

		// Remove the old shell and replace it with the defined class
		class_hash.Remove(tclass);
		class_hash.Add(class_ptr->GetName(), class_ptr);
	}

	// Add to the list of base classes if it's a base class
	if (!class_ptr->DoesInherit())
		base_classes.Add(class_ptr);

	outfunc;
}


void Environment::AddObject(const char *filename)
{
	infunc(Environment::AddObject);

	int		offset;
	Class	*class_ptr;

	// Open the object file for reading
	if ((file = new CFile(cur_filename = filename, FILEOPEN_READ)) == NULL)
		throw CError("Couldn't allocate a new object file");

	// Read where the code segment ends
	file->SeekTo(-4, FILESEEK_END);
	file->Read(&offset, 4);
	file->SeekTo(offset, FILESEEK_START);

	// Allocate the new class
	if ((class_ptr = new Class) == NULL)
		throw CError("Couldn't allocate new class");

	// Read the class information from file
	class_ptr->Read(file);

	// The class has been defined so set it
	class_ptr->SetDefined();

	// Add it!
	AddClassPtr(class_ptr);

	// Close the file
	delete file;

	// Set the output filename
	class_ptr->SetObjectFile((char *)filename);

	outfunc;
}


void Environment::ConstructClass(const char *filename)
{
	infunc(Environment::ConstructClass);

	char	*data;
	int		size;

	// Open the VMC file for reading
	if ((file = new CFile(cur_filename = filename, FILEOPEN_READ)) == NULL)
		throw CError("Couldn't allocate a new VMC file");

	size = file->GetSize();
	g_Object = NULL;

	// Allocate some space to store the file
	if ((data = new char[size]) == NULL)
		throw CError("Couldn't allocate space for the VMC file");

	// Read the entire file into the buffer
	file->Read(data, size);

	// Create the tokeniser and code generator

	if ((tokeniser = new Tokeniser(data, size, TOKENISER_VMC)) == NULL)
		throw CError("Couldn't allocate a tokeniser");

	if ((code_gen = new CodeGenerator(tokeniser, data)) == NULL)
		throw CError("Couldn't allocate a code generator");

	// Being parsing the code
	code_gen->Begin();

	// Mark this class for compilation
	cur_class->SetForCompile((char *)filename);
	cur_class = NULL;

	// Delete the tokeniser and generator
	delete code_gen;
	delete tokeniser;

	// No longer need the data
	delete data;

	// Close the file
	delete file;

	outfunc;
}


void Environment::CompileClass(const char *filename, const char *output)
{
	infunc(Environment::CompileClass);

	char	*data;
	int		size;

	// Open the VMC file for reading
	if ((file = new CFile(cur_filename = filename, FILEOPEN_READ)) == NULL)
		throw CError("Couldn't allocate a new VMC file");

	size = file->GetSize();

	// Allocate some space to store the file
	if ((data = new char[size]) == NULL)
		throw CError("Couldn't allocate space for the VMC file");

	// Read the entire file into the buffer
	file->Read(data, size);

	// Create the tokeniser, code generator and object file

	if ((tokeniser = new Tokeniser(data, size, TOKENISER_VMC)) == NULL)
		throw CError("Couldn't allocate a tokeniser");

	if ((code_gen = new CodeGenerator(tokeniser, data)) == NULL)
		throw CError("Couldn't allocate a code generator");

	if ((g_Object = new VMOFile(output)) == NULL)
		throw CError("Couldn't allocate object file");

	// Being parsing the code
	code_gen->Begin();

	// Set the class output file
	cur_class->SetObjectFile((char *)output);

	g_Object->Disassemble();

	// Delete the tokeniser, generator and object file
	delete g_Object;
	delete code_gen;
	delete tokeniser;
	g_Object = NULL;

	// No longer need the data
	delete data;

	// Close the file
	delete file;

	outfunc;
}


void Environment::CompileClassPtr(Class *class_ptr)
{
	infunc(Environment::CompileClassPtr);

	char			object[PATH_SIZE];
	int				x;

	// Does this class need compiling?
	if (class_ptr->NeedsCompile())
	{
		// Change the filename to that of an object file
		strcpy(object, class_ptr->GetFilename());
		object[strlen(object) - 1] = 'o';

		// Now compile the class
		CompileClass(class_ptr->GetFilename(), object);
	}

	// Work on the childern
	for (x = 0; x < class_ptr->children.GetPosition(); x++)
		CompileClassPtr(class_ptr->children(x));

	outfunc;
}


void Environment::CompileClasses(void)
{
	infunc(Environment::CompileClasses);

	int				x;

	// Loop through all the base classes, compiling them
	for (x = 0; x < base_classes.GetPosition(); x++)
		CompileClassPtr(base_classes(x));

	outfunc;
}


void Environment::LinkClasses(void)
{
	infunc(Environment::LinkClasses);

	Class	*class_ptr, *super_class;
	int		x;

	// Enumerate all classes in the environment
	while (class_ptr = (Class *)class_hash.Enumerate())
	{
		// Does this class inherit?
		if (class_ptr->DoesInherit())
		{
			// Grab a pointer to the super-class
			if ((super_class = (Class *)class_hash.GetEntry(class_ptr->GetSuperClass())) == NULL)
				throw CompileError("Class %s is trying to inherit from a non-existant class %s", class_ptr->GetName(), class_ptr->GetSuperClass());

			// Update the super-class pointer
			class_ptr->sclass_ptr = super_class;

			// Add this class as a child to the super-class
			super_class->children.Add(class_ptr);
		}
	}

	for (x = 0; x < base_classes.GetPosition(); x++)
	{
		// Shift ID's up to make way for super-classes
		base_classes(x)->ShiftFunctionIndices(0);
		base_classes(x)->ShiftImportIndices(0);
		base_classes(x)->ShiftStateIndices(0);
		base_classes(x)->ShiftVariableAddresses(0);
		base_classes(x)->ManageEntryPoint(-1);

		// For virtual objects, resolve over-writes
		base_classes(x)->ResolveFuncOverwrites();
		base_classes(x)->ResolveImpOverwrites();
		base_classes(x)->ResolveStateOverwrites();
	}

	outfunc;
}


void Environment::PrepareForOutput(void)
{
	infunc(Environment::PrepareForOutput);

	int		x;

	for (x = 0; x < base_classes.GetPosition(); x++)
	{
		// Build function and state tables
		base_classes(x)->BuildVTable(NULL);
		base_classes(x)->BuildITable(NULL);
		base_classes(x)->BuildSTable(NULL);
	}

	outfunc;
}


void Environment::FinaliseStates(void)
{
	infunc(Environment::FinaliseStates);

	Class	*cur_class;
	State	*state_ptr;

	// Enumerate all the classes in the environment
	while (cur_class = (Class *)class_hash.Enumerate())
	{
		// Enumerate all the states in this class
		while (state_ptr = (State *)cur_class->states.Enumerate())
		{
			// Check to see if the state beginning has not been updated
			if (state_ptr->GetCodeStart() == -1)
			{
				Class	*class_ptr = cur_class->sclass_ptr;
				State	*state;

				// Search for a valid 'begin' label
				while (class_ptr)
				{
					// See if the state exists in this class
					state = class_ptr->GetState(state_ptr->GetName());

					// If the state exists in this class, see if a start is specified
					if (state && state->GetCodeStart() != -1)
					{
						state_ptr->SetCodeStart(state->GetCodeStart());
						break;
					}

					// Get the super-class
					class_ptr = class_ptr->sclass_ptr;
				}
			}
		}
	}

	outfunc;
}


void Environment::AddObjectToVMU(Class *class_ptr)
{
	infunc(Environment::AddObjectToVMU);

	int		x;

	// Add the object file
	unit->AddObjectCode(class_ptr);

	// Now add each child
	for (x = 0; x < class_ptr->children.GetPosition(); x++)
		AddObjectToVMU(class_ptr->children(x));

	outfunc;
}


void Environment::AddClassToVMU(Class *class_ptr)
{
	infunc(Environment::AddClassToVMU);

	int		x;
	char	*data_segment;

	// Add the class
	unit->AddClassInfo(class_ptr);

	// Allocate the class data segment
	if ((data_segment = new char[class_ptr->GetCurAddress()]) == NULL)
		throw CError("Couldn't allocate data segment");

	// Write the data segment
	unit->WriteDataSegment(class_ptr, data_segment, class_ptr->GetCurAddress());

	// Finished with the data segment
	delete data_segment;

	// Now add each child
	for (x = 0; x < class_ptr->children.GetPosition(); x++)
		AddClassToVMU(class_ptr->children(x));

	outfunc;
}


void Environment::GenerateVMUnit(const char *output)
{
	infunc(Environment::GenerateVMUnit);

	int		x;

	// Open the output file
	if ((unit = new VMUFile(output)) == NULL)
		throw CError("Couldn't allocate VMUnit");

	// Write the code segment
	for (x = 0; x < base_classes.GetPosition(); x++)
		AddObjectToVMU(base_classes(x));

	// Update state inheritance issues ('begin' label)
	FinaliseStates();

	// Write the number of classes
	unit->WriteInt(class_hash.GetNumAddedItems());

	// Write the class information
	for (x = 0; x < base_classes.GetPosition(); x++)
		AddClassToVMU(base_classes(x));

	// Delete the unit
	delete unit;

	outfunc;
}


Class *Environment::GetClass(char *name)
{
	infunc(Environment::GetClass);
	return ((Class *)class_hash.GetEntry(name));
	outfunc;
}


Class *Environment::GetActiveClass(void)
{
	infunc(Environment::GetActiveClass);
	return (cur_class);
	outfunc;
}


void Environment::SetActiveClass(Class *class_ptr)
{
	infunc(Environment::SetActiveClass);
	cur_class = class_ptr;
	outfunc;
}


void Environment::AddIDL(const char *filename)
{
	infunc(Environment::AddIDL);

	int		size;
	char	*data;

	// Open the IDL file for reading
	file = new CFile(filename, FILEOPEN_READ);

	size = file->GetSize();

	// Allocate some space to store the file
	data = new char[size];

	// Read the entire file into the buffer
	file->Read(data, size);

	// Create the tokeniser, code generator and object file

	tokeniser = new Tokeniser(data, size, TOKENISER_IDL);
	idl_parse = new IDLParser(tokeniser, data);

	// Parse the file
	idl_parse->Begin();

	// Close the IDL file
	delete file;

	// Delete stuff
	delete tokeniser;
	delete idl_parse;
	delete data;

	outfunc;
}


void Environment::Debug(void)
{
	infunc(Environment::DebugPrint);

	CFile	*file;
	Class	*class_ptr;

	file = new CFile("debug.out", FILEOPEN_WRITE);

	while (class_ptr = (Class *)class_hash.Enumerate())
	{
		class_ptr->Write(file);
	}

	delete file;

	outfunc;
}