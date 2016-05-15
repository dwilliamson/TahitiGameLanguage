
#include "..\include\main.h"


VMClass::VMClass(CFile *file)
{
	int				x, num;
	char			string[256];

	// Read the class name
	file->ReadStringZ(string);
	m_Name = strnew(string);

	// Read the inheritance information
	if (m_Inherits = file->ReadByte())
	{
		file->ReadStringZ(string);
		m_SuperClass = strnew(string);
	}
	else
	{
		m_SuperClass = NULL;
	}

	// Read the number of variables
	file->Read(&num, sizeof(int));

	for (x = 0; x < num; x++)
	{
		VMVariable	*var;

		// Get the current variable
		if ((var = new VMVariable(file)) == NULL)
			return;

		// Add it to the list
		m_VariableList.AddLast(var);
	}

	// Read the number of functions
	file->Read(&num, sizeof(int));

	for (x = 0; x < num; x++)
	{
		VMFunction	*func;

		// Get the current function
		if ((func = new VMFunction(file)) == NULL)
			return;

		// Add it to the list
		m_FunctionList.AddLast(func);
	}

	// Read the number of imported functions
	file->Read(&num, sizeof(int));

	for (x = 0; x < num; x++)
	{
		VMImport	*import;

		// Get the current imported function
		import = new VMImport(file);

		// Add it to the list
		m_ImportList.AddLast(import);
	}

	// Read the number of states
	file->Read(&num, sizeof(int));

	for (x = 0; x < num; x++)
	{
		VMState	*state;

		// Get the current state
		if ((state = new VMState(file)) == NULL)
			return;

		// Add it to the list
		m_StateList.AddLast(state);
	}

	// Read the class flags
	file->Read(&m_Flags, sizeof(int));

	// Read the class entry point
	file->Read(&m_EntryPoint, sizeof(int));

	// Read the number of functions
	file->Read(&m_NbFunctions, sizeof(int));

	// Allocate and read the virtual function table
	m_FunctionTable = new int[m_NbFunctions];
	file->Read(m_FunctionTable, m_NbFunctions * sizeof(int));

	// Read the number of states
	file->Read(&m_NbStates, sizeof(int));

	// Allocate and read the virtual state table
	m_StateTable = new int[m_NbStates];
	file->Read(m_StateTable, m_NbStates * sizeof(int));

	// Read the size of the data segment
	file->Read(&m_DataSize, sizeof(int));

	// Allocate and read the data segment
	m_DataSegment = new char[m_DataSize];
	file->Read(m_DataSegment, m_DataSize);

	// Set import stuff
	m_NbImports = m_ImportList.GetEntries();
	m_ImportTable = NULL;

	// Defaults
	m_Native = NULL;
}


VMClass::~VMClass(void)
{
	VMVariable	*var;
	VMFunction	*func;
	VMState		*state;

	// Allocated name
	if (m_Name)
		delete [] m_Name;

	// Allocated super-class name
	if (m_SuperClass)
		delete [] m_SuperClass;

	// Allocated data segment
	delete [] m_DataSegment;

	// Allocated function table
	delete [] m_FunctionTable;

	// Allocated state table
	delete [] m_StateTable;

	// Allocated import table
	delete [] m_ImportTable;

	// Delete all the variables in the class
	while (var = m_VariableList.GetFirst())
	{
		m_VariableList.Remove(var);
		delete var;
	}

	// Delete all the functions in the class
	while (func = m_FunctionList.GetFirst())
	{
		m_FunctionList.Remove(func);
		delete func;
	}

	// Delete all the states in the class
	while (state = m_StateList.GetFirst())
	{
		m_StateList.Remove(state);
		delete state;
	}
}


char *VMClass::GetName(void)
{
	return (m_Name);
}


void VMClass::SetUnit(VMUnit *unit_ptr)
{
	m_Unit = unit_ptr;
}


char *VMClass::GetDataSegment(void)
{
	return (m_DataSegment);
}


int *VMClass::GetFunctionTable(void)
{
	return (m_FunctionTable);
}


int *VMClass::GetStateTable(void)
{
	return (m_StateTable);
}


int *VMClass::GetImportTable(void)
{
	return (m_ImportTable);
}


int VMClass::GetNbFunctions(void)
{
	return (m_NbFunctions);
}


int VMClass::GetNbStates(void)
{
	return (m_NbStates);
}


int VMClass::GetNbImports(void)
{
	return (m_NbImports);
}


int VMClass::GetDataSize(void)
{
	return (m_DataSize);
}


int VMClass::GetFlags(void)
{
	return (m_Flags);
}


int VMClass::GetEntryPoint(void)
{
	return (m_EntryPoint);
}


void VMClass::BuildImportTable(void)
{
	int				x;
	VMImport		*import_ptr;
	VirtualMachine	*vm = VirtualMachine::GetInstance();;
	int				method_ptr;

	// Create the import table
	m_ImportTable = new int[m_NbImports];
	import_ptr = m_ImportList.GetFirst();

	// For every imported function
	for (x = 0; x < m_NbImports; x++)
	{
		char	class_name[256];

		// Start with the current class
		strcpy(class_name, m_Name);

		// Loop until a method is found
		while ((method_ptr = (int)vm->GetMethod(class_name, import_ptr->GetName())) == NULL)
		{
			// Move up the class hierarchy
			VMClass	*class_ptr = m_Unit->GetClass(class_name);
			if (!class_ptr->m_Inherits) break;							// IMPORT NOT FOUND!
			strcpy(class_name, class_ptr->m_SuperClass);
		}

		if (method_ptr)
			((NativeMethod *)method_ptr)->m_Import = import_ptr;

		m_ImportTable[x] = method_ptr;
		import_ptr = import_ptr->next;
	}
}