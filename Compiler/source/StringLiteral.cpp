
#include "..\include\main.h"


StringLiteral::StringLiteral(Tokeniser *tokeniser)
{
	infunc(StringLiteral::StringLiteral);

	char	*name;
	int		x;

	// Initialisation
	parent = NULL;
	children[0] = NULL;
	children[1] = NULL;

	// This is a leaf, it doesn't take any types
	in_type.Make(VARIABLE_TYPEID_VOID);

	// There can only this type output by a string literal
	out_type.Make(VARIABLE_TYPEID_STRING);

	// Allocate space for the string literal
	if ((name = new char[CUR_TOKEN.length - 1]) == NULL)
		throw CError("Couldn't allocate string literal");

	// Read the token string and terminate it
	for (x = 1; x < CUR_TOKEN.length - 1; x++)
		name[x - 1] = tokeniser->data[CUR_TOKEN.position + x];
	name[x - 1] = 0;

	// Create the string upon 1st pass
	if (g_Object == NULL)
	{
		// Check to see if the string has already been defined
		string = g_Env->GetActiveClass()->GetStringLiteral(name);

		if (string == NULL)
		{
			int		address;

			// Allocate the variable holder
			if ((string = new Variable) == NULL)
				throw CError("Couldn't allocate string literal variable");

			// Get an address for the string
			address = g_Env->GetActiveClass()->NewAddress(CUR_TOKEN.length - 1, 0);
			string->SetAddress(address);
			string->MakeType(VARIABLE_TYPEID_STRINGLIT);

			// Add the string literal to the class list
			g_Env->GetActiveClass()->string_literals.Add(name, string);
		}
	}

	// Else just grab it
	else
	{
		string = g_Env->GetActiveClass()->GetStringLiteral(name);
	}

	// Release the name memory
	delete [] name;

	INC_TOKEN;

	outfunc;
}


void StringLiteral::GenerateCode(int flags)
{
	infunc(StringLiteral::GenerateCode);

	/* --

	The variable associated with the string literal has an address which points directly
	to the string literals location in the data segment, so push that on the stack.

	-- */

	g_Object->WriteOp(OPCODE_PUSH_MEMBER_ADDRESS, string->GetAddress());

	outfunc;
}


void StringLiteral::ResolveType(int flags)
{
	infunc(StringLiteral::ResolveType);

	// Check for errors only (type was resolved in the constructor)
	if (flags & FLAGS_ASSIGN_DEST)
		throw CompileError("(Line %d) String literals are not l-values", token.line);

	outfunc;
}


int StringLiteral::GetNodeType(void)
{
	infunc(StringLiteral::GetNodeType);

	// Just a fancy constant
	return (PTNODE_TYPE_CONSTANT);

	outfunc;
}