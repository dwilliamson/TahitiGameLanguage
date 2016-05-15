
#include "..\include\main.h"


State::State(void)
{
	position = -1;
	code_start = -1;
}


State::~State(void)
{
	Function	*func;

	// Delete all the functions
	while (func = (Function *)functions.Enumerate())
	{
		functions.Remove(func);
		delete func;
	}
}


void State::SetPosition(int where)
{
	position = where;
}


int State::GetPosition(void)
{
	return (position);
}


void State::SetCodeStart(int where)
{
	code_start = where;
}


int State::GetCodeStart(void)
{
	return (code_start);
}


void State::SetIndex(int value)
{
	index = value;
}


int State::GetIndex(void)
{
	return (index);
}


void State::Write(CFile *file)
{
	infunc(State::Write);

	int				pos, backup, x;
	Function		*func_ptr;

	// Name
	file->PrintfZ(GetName());

	// Where the state is located in code
	file->Write(&position, 4);

	// Location of the 'begin' label
	file->Write(&code_start, 4);

	// State index within the class
	file->Write(&index, 4);

	// Record the beginning of function info
	pos = file->GetPosition();
	file->WriteDWordLoop(0, 1);
	x = 0;

	// Enumerate all the functions in this state
	while ((func_ptr = (Function *)functions.Enumerate()))
	{
		func_ptr->Write(file);
		x++;
	}

	// Write the number of functions in the state
	backup = file->GetPosition();
	file->SeekTo(pos, FILESEEK_START);
	file->Write(&x, 4);
	file->SeekTo(backup, FILESEEK_START);

	outfunc;
}


void State::Read(CFile *file)
{
	infunc(State::Read);

	int		num, x;
	char	string[MAX_LABEL_SIZE];

	// State name
	file->ReadStringZ(string);
	SetName(string);

	// Position in code where state starts
	file->Read(&position, 4);

	// Where to start executing in this state
	file->Read(&code_start, 4);

	// State index within the class
	file->Read(&index, 4);

	// Number of functions in the class
	file->Read(&num, 4);

	// For every function
	for (x = 0; x < num; x++)
	{
		Function	*func;

		// Allocate some space
		if ((func = new Function) == NULL)
			throw CError("Couldn't allocate function");

		// Read the function from file
		func->Read(file);

		// Add to the list of functions
		functions.Add(func->GetName(), func);
	}

	outfunc;
}


void State::WriteVMUInfo(CFile *file)
{
	infunc(State::WriteVMUInfo);

	Function	*func;
	int			num, pos;

	// Name
	file->PrintfZ(GetName());

	// Where the state is located in code
	file->Write(&position, 4);

	// Location of the 'begin' label
	file->Write(&code_start, 4);

	// State index within the class
	file->Write(&index, 4);

	// Remember where the number of functions is stored
	pos = file->GetPosition();
	file->WriteDWordLoop(0, 1);
	num = 0;

	// Enumerate all the functions in the state, writing them
	while (func = (Function *)functions.Enumerate())
	{
		func->WriteVMUInfo(file);
		num++;
	}

	// Write the number of functions
	g_Unit->BackpatchInt(num, pos);

	outfunc;
}