
#include "..\include\main.h"


Variable::Variable(void)
{
	infunc(Variable::Variable);

	scope = 0;
	flags = 0;
	type.id = 0;
	type.array = 0;
	type.elements = 0;
	default_value.i = 0;
	array_values = NULL;

	outfunc;
}


void Variable::SetScope(int value)
{
	scope = value;
}


int Variable::GetScope(void)
{
	return (scope);
}


void Variable::SetArrayElements(int value)
{
	type.elements = value;

	if (type.elements != -1)
	{
		// Allocate some space for the default values
		if ((array_values = new dynamic[type.elements]) == NULL)
			throw CError("Couldn't allocate array elements");

		// Clear it out
		memset(array_values, 0, type.elements * sizeof(int));
	}
}


int Variable::GetArrayElements(void)
{
	return (type.elements);
}


void Variable::SetAddress(int value)
{
	address = value;
}


int Variable::GetAddress(void)
{
	return (address);
}


void Variable::SetFlag(int value)
{
	flags |= value;
}


void Variable::ClearFlag(int value)
{
	flags &= ~value;
}


int Variable::GetFlags(void)
{
	return (flags);
}


void Variable::SetDefaultValue(void *value)
{
	default_value.ui = *(unsigned int *)value;
}


void Variable::GetDefaultValue(void *value)
{
	*(unsigned int *)value = default_value.ui;
}


void Variable::SetElementValue(int index, void *value)
{
	if (array_values)
		array_values[index].ui = *(unsigned int *)value;
}


void Variable::GetElementValue(int index, void *value)
{
	if (array_values)
		*(unsigned int *)value = array_values[index].ui;
}


VarType &Variable::GetType(void)
{
	return (type);
}


void Variable::SetType(VarType &_type)
{
	type = _type;
}


void Variable::MakeType(int id)
{
	type.Make(id);
}


void Variable::Write(CFile *file)
{
	infunc(Variable::Write);

	// Name
	file->PrintfZ(GetName());

	// Address of the variable relative to the class
	file->Write(&address, 4);

	// Write the type ID
	file->Write(&type.id, 4);

	// Write the variable type if it's custom
	if (type.id == VARIABLE_TYPEID_CUSTOM)
		file->PrintfZ(type.name);

	// Write if it's an array
	file->WriteByte(type.array);

	// Write the number of elements
	if (type.array)
		file->Write(&type.elements, 4);

	// Write the default values
	if (type.array)
		file->Write(array_values, 4 * type.elements);
	else
		file->Write(&default_value, 4);

	// Variable flags
	file->Write(&flags, 4);

	outfunc;
}


void Variable::Read(CFile *file)
{
	infunc(Variable::Read);

	char	string[MAX_LABEL_SIZE];

	// Variable name
	file->ReadStringZ(string);
	SetName(string);

	// Address of the variable relative to the class
	file->Read(&address, 4);

	// Type ID
	file->Read(&type.id, 4);

	// Read the type name if it's custom
	if (type.id == VARIABLE_TYPEID_CUSTOM)
		file->ReadStringZ(type.name);

	// If it's an array
	type.array = (int)file->ReadByte();

	// Read the number of elements if it's an array
	if (type.array)
		file->Read(&type.elements,4);

	// Read all the default values
	if (type.array)
	{
		int		x;

		// Allocate some space for the default values
		if ((array_values = new dynamic[type.elements]) == NULL)
			throw CError("Couldn't allocate array elements");

		for (x = 0; x < type.elements; x++)
			file->Read(&array_values[x], 4);
	}
	else
	{
		file->Read(&default_value, 4);
	}

	// Variable flags
	file->Read(&flags, 4);

	outfunc;
}


void Variable::WriteVMUInfo(CFile *file)
{
	infunc(Variable::WriteVMUInfo);

	// Name
	file->PrintfZ(GetName());

	// Address of the variable relative to the class
	file->Write(&address, 4);

	// Write the type ID
	file->Write(&type.id, 4);

	// Write the variable type if it's custom
	if (type.id == VARIABLE_TYPEID_CUSTOM)
		file->PrintfZ(type.name);

	// Write if it's an array
	file->WriteByte(type.array);

	// Write the number of elements
	if (type.array)
		file->Write(&type.elements, 4);

	outfunc;
}