
#include "..\include\main.h"


#define	CLASS_FLAGS_ABSTRACT	0x0001
#define CLASS_FLAGS_INTERFACE	0x0002


Class::Class(CFile *file)
{
	int		x;

	// Read the class name
	file->ReadStringZ(m_Name);

	// Read the inheritance information
	if (m_Inherits = file->ReadByte())
		file->ReadStringZ(m_SuperClass);

	// Read the number of variables
	file->Read(&m_NbVariables, sizeof(int));

	for (x = 0; x < m_NbVariables; x++)
	{
		Variable	*var;

		// Get the current variable
		var = new Variable(file);

		// Add the variable to the list
		m_VariableList.Add(var->m_Name, var);
	}

	// Read the number of functions
	file->Read(&m_NbFunctions, sizeof(int));

	for (x = 0; x < m_NbFunctions; x++)
	{
		Function	*func;

		// Get the current function
		func = new Function(file);
		func->m_Import = 0;

		// Add the function to the list
		m_FunctionList.Add(func->m_Name, func);
	}

	// Read the number of imported functions
	file->Read(&m_NbImports, sizeof(int));

	for (x = 0; x < m_NbImports; x++)
	{
		Function	*func;

		// Get the current function
		func = new Function(file);
		func->m_Import = 1;

		// Add the function to the import list
		m_ImportList.Add(func);
	}

	// Read the number of states
	file->Read(&m_NbStates, sizeof(int));

	for (x = 0; x < m_NbStates; x++)
	{
		State	*state;

		// Get the current state
		state = new State(file);

		// Add the state to the list
		m_StateList.Add(state->m_Name, state);
	}

	// Class flags
	file->Read(&m_Flags, sizeof(int));

	// Code entry point
	file->Read(&m_EntryPoint, sizeof(int));

	// Read the vtable
	file->Read(&x, sizeof(int));
	if (x) m_VTable = new Table(file, x, TABLE_TYPE_FUNCTIONS);
	else m_VTable = NULL;

	// Seek passed the stable
	file->Read(&x, sizeof(int));
	if (x) m_STable = new Table(file, x, TABLE_TYPE_STATES);
	else m_STable = NULL;

	// Seek passed the data segment
	file->Read(&x, sizeof(int));
	file->SeekTo(x, FILESEEK_CURRENT);
}


Class::~Class(void)
{
	int			x;
	Variable	*var;
	Function	*func;
	State		*state;

	// Delete all the variables
	while (var = (Variable *)m_VariableList.Enumerate())
	{
		m_VariableList.Remove(var);
		delete var;
	}

	// Delete all the functions
	while (func = (Function *)m_FunctionList.Enumerate())
	{
		m_FunctionList.Remove(func);
		delete func;
	}

	// Delete all the imported functions
	for (x = 0; x < m_ImportList.GetPosition(); x++)
		delete m_ImportList(x);

	// Delete all the states
	while (state = (State *)m_StateList.Enumerate())
	{
		m_StateList.Remove(state);
		delete state;
	}

	// Delete the tables
	if (m_VTable) delete m_VTable;
	if (m_STable) delete m_STable;
}


void Class::AddToTreeView(int level)
{
	int			x;
	Variable	*var;
	Function	*func;
	State		*state;

	// Add this class to the treeview
	if (m_Flags & CLASS_FLAGS_INTERFACE)
	{
		if (!(g_Display & DISPLAY_INTERFACE))
			return;

		g_Tree->AddInterface(m_Name, level, this);
	}
	else
	{
		if (!(g_Display & DISPLAY_CLASS))
			return;

		g_Tree->AddClass(m_Name, level, this);
	}

	// Add all the children
	for (x = 0; x < m_Children.GetPosition(); x++)
		m_Children(x)->AddToTreeView(level + 1);

	// Add all the variables
	while (var = (Variable *)m_VariableList.Enumerate())
	{
		if (var->m_TypeID == VARIABLE_TYPEID_CUSTOM)
		{
			// Can display objects?
			if (g_Display & DISPLAY_OBJECT)
				g_Tree->AddObject(var->m_Name, level + 1, var);
		}
		else
		{
			// Can display variables?
			if (g_Display & DISPLAY_VARIABLE)
				g_Tree->AddVariable(var->m_Name, level + 1, var);
		}
	}

	// Can display functions?
	if (g_Display & DISPLAY_FUNCTION)
	{
		// Add all the functions
		while (func = (Function *)m_FunctionList.Enumerate())
			g_Tree->AddFunction(func->m_Name, level + 1, func);
	}

	// Can display imported functions?
	if (g_Display & DISPLAY_IMPORT)
	{
		// Add all the imported functions
		for (x = 0; x < m_ImportList.GetPosition(); x++)
			g_Tree->AddImport(m_ImportList(x)->m_Name, level + 1, m_ImportList(x));
	}

	// Can display states?
	if (g_Display & DISPLAY_STATE)
	{
		// Add all the states
		while (state = (State *)m_StateList.Enumerate())
			state->AddToTreeView(level + 1);
	}

	// Can display tables?
	if (g_Display & DISPLAY_TABLE)
	{
		if (m_VTable) m_VTable->AddToTreeView(level + 1);
		if (m_STable) m_STable->AddToTreeView(level + 1);
	}
}


void Class::PrintInfo(void)
{
	// Type
	if (m_Flags & CLASS_FLAGS_INTERFACE)
	{
		g_Edit->Printf("INTERFACE\n");
		g_Edit->Printf("\n");
	}
	else
	{
		g_Edit->Printf("CLASS\n");
		g_Edit->Printf("\n");
	}

	// Class name info
	g_Edit->Printf("Name: %s\n", m_Name);
	if (m_Inherits) g_Edit->Printf("Super-class: %s\n", m_SuperClass);
	g_Edit->Printf("\n");

	// Count info
	if (!(m_Flags & CLASS_FLAGS_INTERFACE))
	{
		g_Edit->Printf("Number of variables: %d\n", m_NbVariables);
		g_Edit->Printf("Number of functions: %d\n", m_NbFunctions);
		g_Edit->Printf("Number of states: %d\n", m_NbStates);
	}
	g_Edit->Printf("Number of imported functions: %d\n", m_NbImports);
	g_Edit->Printf("\n");

	// Entry point
	if (!(m_Flags & CLASS_FLAGS_INTERFACE))
	{
		g_Edit->Printf("Entry point: %d\n", m_EntryPoint);
		g_Edit->Printf("\n");
	}

	// Class flags
	g_Edit->Printf("Flags:\n");
	PRINT_FLAGS(m_Flags, CLASS_FLAGS_ABSTRACT);
	PRINT_FLAGS(m_Flags, CLASS_FLAGS_INTERFACE);
	g_Edit->Printf("\n");
}