
#include "..\include\main.h"


VMState::VMState(CFile *file)
{
	char	string[256];
	int		x, num;

	// Read the state name
	file->ReadStringZ(string);
	m_Name = strnew(string);

	// Read where the state is located
	file->Read(&m_Position, sizeof(int));

	// Read where the 'begin' label is
	file->Read(&m_CodeStart, sizeof(int));

	// Read the index within the state
	file->Read(&m_Index, sizeof(int));

	// Number of functions in the state
	file->Read(&num, sizeof(int));

	for (x = 0; x < num; x++)
	{
		VMFunction	*func;

		// Get the function
		if ((func = new VMFunction(file)) == NULL)
			return;

		// Add it to the list
		m_FunctionList.AddLast(func);
	}
}


VMState::~VMState(void)
{
	VMFunction	*func;

	// Allocated state name
	if (m_Name)
		delete [] m_Name;

	// Delete all the functions in the state
	while (func = m_FunctionList.GetFirst())
	{
		m_FunctionList.Remove(func);
		delete func;
	}
}