
#include "..\include\main.h"


Constant::Constant(char *token_string)
{
	infunc(Constant::Constant);

	// Initialisation
	parent = NULL;
	children[0] = NULL;
	children[1] = NULL;

	// This is a leaf, it doesn't take any types
	in_type.Make(VARIABLE_TYPEID_VOID);

	// Is it a floating point number?
	if (strchr(token_string, '.'))
	{
		value.f = (float)atof(token_string);
		out_type.Make(VARIABLE_TYPEID_FLOAT);
	}

	// No
	else
	{
		int				num;

		// Read the number as an integer
		num = atoi(token_string);

		// If the result is negative then it overflowed, must be unsigned
		if (num < 0)
		{
			sscanf(token_string, "%u", &value.ui);
			out_type.Make(VARIABLE_TYPEID_UINT);
		}
		else
		{
			value.i = num;
			out_type.Make(VARIABLE_TYPEID_INT);
		}
	}

	outfunc;
}


void Constant::GenerateCode(int flags)
{
	infunc(Constant::GenerateCode);

	// Constants can only be one of three types
	switch (out_type.id)
	{
		case (VARIABLE_TYPEID_INT):
			g_Object->WriteOp(OPCODE_PUSH_IMMEDIATE, value.i);
			break;

		case (VARIABLE_TYPEID_UINT):
			g_Object->WriteOp(OPCODE_PUSH_IMMEDIATE, value.ui);
			break;

		case (VARIABLE_TYPEID_FLOAT):
			g_Object->WriteOp(OPCODE_FPUSH_IMMEDIATE, value.f);
			break;
	}

	outfunc;
}


void Constant::ResolveType(int flags)
{
	infunc(Constant::ResolveType);
	// The type has already been figured out by the constructor
	(flags == flags);
	outfunc;
}


int Constant::GetNodeType(void)
{
	infunc(Constant::GetNodeType);

	return (PTNODE_TYPE_CONSTANT);

	outfunc;
}