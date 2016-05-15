
#include "..\include\main.h"


void VarType::operator = (VarType &other)
{
	infunc(VarType::operator=);

	id = other.id;
	strcpy(name, other.name);
	array = other.array;
	elements = other.elements;

	outfunc;
}


int VarType::operator == (VarType &other)
{
	infunc(VarType::operator==);

	if (id != other.id)
		return (0);

	if (array != other.array)
		return (0);

	if (elements != other.elements)
		return (0);

	if (strcmp(name, other.name))
		return (0);

	return (1);

	outfunc;
}


int VarType::operator != (VarType &other)
{
	infunc(VarType::operator!=);

	return (!(*this == other));

	outfunc;
}


void VarType::Make(int what)
{
	infunc(VarType::Make);

	id = what;
	array = 0;
	elements = 0;

	switch (what)
	{
		case (VARIABLE_TYPEID_VOID): strcpy(name, "void"); break;
		case (VARIABLE_TYPEID_CHAR): strcpy(name, "char"); break;
		case (VARIABLE_TYPEID_UCHAR): strcpy(name, "unsigned char"); break;
		case (VARIABLE_TYPEID_SHORT): strcpy(name, "short"); break;
		case (VARIABLE_TYPEID_USHORT): strcpy(name, "unsigned short"); break;
		case (VARIABLE_TYPEID_INT): strcpy(name, "int"); break;
		case (VARIABLE_TYPEID_UINT): strcpy(name, "unsigned int"); break;
		case (VARIABLE_TYPEID_FLOAT): strcpy(name, "float"); break;
		case (VARIABLE_TYPEID_BINARY): strcpy(name, "binary"); break;
		case (VARIABLE_TYPEID_STRING): strcpy(name, "string"); break;
		case (VARIABLE_TYPEID_STRINGLIT): strcpy(name, "string literal"); break;
	}

	outfunc;
}