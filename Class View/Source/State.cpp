
#include "..\include\main.h"


State::State(CFile *file)
{
	int		x;

	// Read the state name
	file->ReadStringZ(m_Name);

	// Read where the state is located
	file->Read(&m_Position, sizeof(int));

	// Read where the 'begin' label is
	file->Read(&m_CodeStart, sizeof(int));

	// Read the index within the state
	file->Read(&m_Index, sizeof(int));

	// Number of functions in the state
	file->Read(&m_NbFunctions, sizeof(int));

	for (x = 0; x < m_NbFunctions; x++)
	{
		Function	*func;

		// Get the current function
		func = new Function(file);

		// Add it to the list
		m_FunctionList.Add(func->m_Name, func);
	}
}


State::~State(void)
{
	Function	*func;

	// Delete all the functions
	while (func = (Function *)m_FunctionList.Enumerate())
	{
		m_FunctionList.Remove(func);
		delete func;
	}
}


void State::AddToTreeView(int level)
{
	Function	*func;

	// Add the state
	g_Tree->AddState(m_Name, level, this);

	// Can display functions?
	if (!(g_Display & DISPLAY_FUNCTION))
		return;

	// Add all the functions in the state
	while (func = (Function *)m_FunctionList.Enumerate())
		g_Tree->AddFunction(func->m_Name, level + 1, func);
}


void State::PrintInfo(void)
{
	// Type
	g_Edit->Printf("STATE\n");
	g_Edit->Printf("\n");

	// Name
	g_Edit->Printf("Name: %s\n", m_Name);
	g_Edit->Printf("\n");

	// Misc.
	g_Edit->Printf("Code position: %d\n", m_Position);
	g_Edit->Printf("Code start: %d\n", m_CodeStart);
	g_Edit->Printf("Index within class: %d\n", m_Index);
	g_Edit->Printf("Number of functions: %d\n", m_NbFunctions);
}