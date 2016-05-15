
#include "..\include\main.h"


Name::Name(Tokeniser *_tokeniser)
{
	infunc(Name::Name);

	// Save the name token
	tokeniser = _tokeniser;
	token = PREV_TOKEN;

	// Set some default values
	variable = NULL;
	children[0] = NULL;
	children[1] = NULL;
	parent = NULL;

	outfunc;
}


Name::~Name(void)
{
}


void Name::GenerateCode(int flags)
{
	infunc(Name::GenerateCode);

	/* --------------

	If the variable in question is an array, push the value on the stack since that value
	is a pointer to the array. Arrays need to be modified later by the index so it's the
	pointer that needs to be pushed.

	Non-array variables. If they are involved in an assignment then the value needs to be
	pushed on the stack. If the variables are the destination of an assignment then the
	address needs to be pushed on the stack (for the ASG instruction). When it's a coupled
	assigment and operator, the RPV instruction needs to be used which pushes the address
	down one place in the stack and replaces it with the value stored at that address so
	the operation can be performed, followed by the assignment.

	The (++, --) operators need the address on the stack so simply tell the variable
	that it is the destination of an assignment so that the address is pushed.

	Assignment destination is checked for before assignment source to cater for events
	such as "a = b = c". This forces a push of b's address instead of the value. The
	same can be found for a token array open token.

	-------------- */

	if (out_type.array)
		PushValue(1);
	else
	{
		if (flags & FLAGS_ASSIGN_DEST)
			PushAddress();

		else if (flags & FLAGS_ASSIGNMENT)
			PushValue(0);

		else if (flags & FLAGS_ASSIGNOP_DEST)
		{
			PushAddress();
			g_Object->WriteOp(OPCODE_RPV);
		}
	}

	outfunc;
}


void Name::ResolveType(int flags)
{
	infunc(Name::ResolveType);

	char	name[MAX_LABEL_SIZE];

	// Get the variable name
	tokeniser->IsolateString(token, name);

	if (variable == NULL)
	{
		// Get the variable pointer and release the string memory
		if ((variable = g_Env->GetActiveClass()->GetVariable(name, flags)) == NULL)
			throw CompileError("(Line %d) Undefined label '%s'", token.line, name);
	}

	// Get the type from the variable
	out_type = variable->GetType();
	in_type.Make(VARIABLE_TYPEID_VOID);

	outfunc;
}


void Name::PushAddress(void)
{
	infunc(Name::PushAddress);

	int		scope;

	scope = variable->GetScope();

	// Push the address
	if (scope == VARIABLE_SCOPE_LOCAL || scope == VARIABLE_SCOPE_PARAMETER)
		g_Object->WriteOp(OPCODE_PUSH_LOCAL_ADDRESS, variable->GetAddress());
	else
		g_Object->WriteOp(OPCODE_PUSH_MEMBER_ADDRESS, variable->GetAddress());

	outfunc;
}


void Name::PushValue(int array)
{
	infunc(Name::PushValue);

	int		scope;

	scope = variable->GetScope();

	if (array)
	{
		// Push the value
		if (scope == VARIABLE_SCOPE_LOCAL || scope == VARIABLE_SCOPE_PARAMETER)
			g_Object->WriteOp(OPCODE_PUSH_LOCAL_ARRAY, variable->GetAddress());
		else
			g_Object->WriteOp(OPCODE_PUSH_MEMBER_ARRAY, variable->GetAddress());
	}
	else
	{
		// Push the value
		if (scope == VARIABLE_SCOPE_LOCAL || scope == VARIABLE_SCOPE_PARAMETER)
			g_Object->WriteOp(OPCODE_PUSH_LOCAL_VALUE, variable->GetAddress());
		else
			g_Object->WriteOp(OPCODE_PUSH_MEMBER_VALUE, variable->GetAddress());
	}

	outfunc;
}
