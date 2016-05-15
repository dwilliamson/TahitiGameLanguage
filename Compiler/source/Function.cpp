
#include "..\include\main.h"


Function::Function(void)
{
	nb_params = 0;
	cur_paddress = 0;
	cur_address = 0;
	flags = FFLAGS_PUBLIC;
}


int Function::GetNumParams(void)
{
	return (nb_params);
}


Variable *Function::AddParameter(VarType &type, char *name)
{
	infunc(Function::AddParameter);

	Variable	*var;

	// Create the new variable
	if ((var = new Variable) == NULL)
		throw CError("Couldn't allocate parameter variable");

	var->SetType(type);
	var->SetScope(VARIABLE_SCOPE_PARAMETER);

	// Set the address relative to the stack pointer
	var->SetAddress(cur_paddress);
	cur_paddress += 4;
	nb_params++;

	local_list.Add(name, var);
	param_list.Add(var);

	return (var);

	outfunc;
}


Variable *Function::AddLocal(VarType &type, char *name)
{
	infunc(Function::AddLocal);

	Variable	*var;

	// Create the new variable
	if ((var = new Variable) == NULL)
		throw CError("Couldn't allocate local variable structure");

	var->SetType(type);
	var->SetScope(VARIABLE_SCOPE_LOCAL);
	var->SetAddress(cur_address);

	if (type.array)
		cur_address += (4 + 4 * type.elements);
	else
		cur_address += 4;

	local_list.Add(name, var);

	return (var);

	outfunc;
}


Variable *Function::GetParameter(int which)
{
	infunc(Function::GetParameter);
	return (param_list(which));
	outfunc;
}


int Function::GetCurAddress(void)
{
	return (cur_address);
}


int Function::GetCurPAddress(void)
{
	return (cur_paddress);
}


void Function::FixAddresses(void)
{
	infunc(Function::FixAddresses);

	Variable	*var;

	// Enumerate all the variables
	while (var = (Variable *)local_list.Enumerate())
	{
		// Is this a parameter?
		if (var->GetScope() == VARIABLE_SCOPE_PARAMETER)
		{
			// Calculate the new address
			var->SetAddress(0 - ((cur_paddress - 4) - var->GetAddress()) - 12);
		}
	}

	outfunc;
}


void Function::SingulariseAddresses(void)
{
	infunc(Function::SingulariseAddresses);

	Variable	*var;

	// Enumerate all the variables
	while (var = (Variable *)local_list.Enumerate())
	{
		// Is this a parameter?
		if (var->GetScope() == VARIABLE_SCOPE_PARAMETER)
		{
			// Calculate the new address
			var->SetAddress(var->GetAddress() >> 2);
		}
	}

	outfunc;
}


int Function::GetCodeLocation(void)
{
	return (location);
}


void Function::SetCodeLocation(int where)
{
	location = where;
}


void Function::SetIndex(int value)
{
	index = value;
}


int Function::GetIndex(void)
{
	return (index);
}


void Function::SetFlag(int which)
{
	flags |= which;
}


void Function::ClearFlag(int which)
{
	flags &= ~which;
}


int Function::GetFlags(void)
{
	return (flags);
}


VarType Function::GetReturnType(void)
{
	return (ret_type);
}


void Function::SetReturnType(VarType &type)
{
	ret_type = type;
}


void Function::WriteReturn(VMOFile *object)
{
	infunc(Function::WriteReturn);

	// Reclaim the allocated stack and backpatch the addition
	if (cur_address) object->WriteOp(OPCODE_ADDST,  cur_address);

	// Exit stack frame
	if (cur_address || cur_paddress) object->WriteOp(OPCODE_OUTSTF);

	// Return
	object->WriteOp(OPCODE_RET);

	// Set this everytime
	had_return = 1;

	outfunc;
}


void Function::Write(CFile *file)
{
	infunc(Function::Write);

	Variable	*var;

	// Name
	file->PrintfZ(GetName());

	// Index within the class
	file->Write(&index, 4);

	// Code location
	file->Write(&location, 4);

	// Return type ID
	file->Write(&ret_type.id, 4);

	// Type name if it's a custom variable
	if (ret_type.id == VARIABLE_TYPEID_CUSTOM)
		file->PrintfZ(ret_type.name);

	// If it's an array
	file->WriteByte(ret_type.array);

	// Number of elements in the array
	if (ret_type.array)
		file->Write(&ret_type.elements, 4);

	// Number of parameters
	file->Write(&nb_params, 4);

	// Enumerate all the local variables
	while ((var = (Variable *)local_list.Enumerate()))
	{
		// Check only for function parameters
		if (var->GetScope() == VARIABLE_SCOPE_PARAMETER)
		{
			VarType		type = var->GetType();

			// Type ID
			file->Write(&type.id, 4);

			// Write the type name if it's custom
			if (type.id == VARIABLE_TYPEID_CUSTOM)
				file->PrintfZ(type.name);

			// If it's an array
			file->WriteByte(type.array);

			// Number of array elements
			if (type.array)
				file->Write(&type.elements, 4);
		}
	}

	// Function flags
	file->Write(&flags, 4);

	outfunc;
}


void Function::Read(CFile *file)
{
	infunc(Function::Read);

	int		num, x;
	char	string[MAX_LABEL_SIZE];

	// Function name
	file->ReadStringZ(string);
	SetName(string);

	// Index within the class
	file->Read(&index, 4);

	// Code address
	file->Read(&location, 4);

	// Return type ID
	file->Read(&ret_type.id, 4);

	// Read the return type name if it's a custom variable
	if (ret_type.id == VARIABLE_TYPEID_CUSTOM)
		file->ReadStringZ(ret_type.name);

	// If the return type is an array
	ret_type.array = (int)file->ReadByte();

	// How many elements there are in the array
	if (ret_type.array)
		file->Read(&ret_type.elements, 4);
	else
		ret_type.elements = 0;

	// Number of parameters
	file->Read(&num, 4);

	// For every parameter
	for (x = 0; x < num; x++)
	{
		Variable	*var;
		VarType		type;

		// Allocate some space
		if ((var = new Variable) == NULL)
			throw CError("Couldn't allocate variable");

		// Type ID
		file->Read(&type.id, 4);

		// Type name if it's custom
		if (type.id == VARIABLE_TYPEID_CUSTOM)
			file->ReadStringZ(type.name);

		// If it's an array
		type.array = (int)file->ReadByte();

		// Number of elements in the array
		if (type.array)
			file->Read(&type.elements, 4);
		else
			type.elements = 0;
	}

	// Function flags
	file->Read(&flags, 4);

	outfunc;
}


void Function::WriteVMUInfo(CFile *file)
{
	infunc(Function::WriteVMUInfo);

	Variable	*var;

	// Name
	file->PrintfZ(GetName());

	// Index within the class
	file->Write(&index, 4);

	// Code location
	file->Write(&location, 4);

	// Return type ID
	file->Write(&ret_type.id, 4);

	// Type name if it's a custom variable
	if (ret_type.id == VARIABLE_TYPEID_CUSTOM)
		file->PrintfZ(ret_type.name);

	// If it's an array
	file->WriteByte(ret_type.array);

	// Number of elements in the array
	if (ret_type.array)
		file->Write(&ret_type.elements, 4);

	// Number of parameters
	file->Write(&nb_params, 4);

	// Enumerate all the local variables
	while ((var = (Variable *)local_list.Enumerate()))
	{
		// Check only for function parameters
		if (var->GetScope() == VARIABLE_SCOPE_PARAMETER)
		{
			VarType		type = var->GetType();

			// Type ID
			file->Write(&type.id, 4);

			// Write the type name if it's custom
			if (type.id == VARIABLE_TYPEID_CUSTOM)
				file->PrintfZ(type.name);

			// If it's an array
			file->WriteByte(type.array);

			// Number of array elements
			if (type.array)
				file->Write(&type.elements, 4);
		}
	}

	outfunc;
}
