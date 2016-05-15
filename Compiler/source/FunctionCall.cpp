
#include "..\include\main.h"


FunctionCall::FunctionCall(Tokeniser *_tokeniser)
{
	infunc(FunctionCall::FunctionCall);

	// Save the name token
	tokeniser = _tokeniser;
	token = PREV_TOKEN;

	// Initialisation
	function = NULL;
	parent = NULL;
	children[0] = NULL;
	children[1] = NULL;

	// Skip passed the open bracket
	INC_TOKEN;

	while (1)
	{
		Expression	*expr;

		// Check for the end
		if (CUR_TOKEN.type == TOKEN_RIGHT_BRACKET)
		{
			INC_TOKEN;
			break;
		}

		// Process this expression
		if ((expr = new Expression(tokeniser, TOKEN_RIGHT_BRACKET, TOKEN_COMMA)) == NULL)
			throw CError("Couldn't allocate expression structure");

		// Add the expression to the list
		param_list.Add(expr);

		// Go passed the terminator
		INC_TOKEN;

		// Check for the end
		if (PREV_TOKEN.type == TOKEN_RIGHT_BRACKET)
			break;
	}

	outfunc;
}


FunctionCall::~FunctionCall(void)
{
	infunc(FunctionCall::~FunctionCall);

	int		x;

	// Delete each parameter expression
	for (x = 0; x < param_list.GetPosition(); x++)
		delete param_list(x);

	outfunc;
}


void FunctionCall::GenerateCode(int flags)
{
	infunc(FunctionCall::GenerateCode);

	int		x;

	// Imported function call
	if (function->GetFlags() & FFLAGS_IMPORTED)
	{
		// Push each parameter on the stack from right to left (as C expects them)
		for (x = param_list.GetPosition() - 1; x >= 0; x--)
		{
			param_list(x)->GenerateCode(flags | FLAGS_IMPLICIT_ASSIGNMENT);
			WriteConverter(param_list(x)->out_type.id, function->GetParameter(x)->GetType().id);
		}

		// Write the function call
		g_Object->WriteOp(OPCODE_CALLIM, function->GetIndex());

		// Allocated stack memory is automatically reclaimed by the instruction
	}

	// Normal function call
	else
	{
		// Push each parameter on the stack
		for (x = 0; x < param_list.GetPosition(); x++)
		{
			param_list(x)->GenerateCode(flags | FLAGS_IMPLICIT_ASSIGNMENT);
			WriteConverter(param_list(x)->out_type.id, function->GetParameter(x)->GetType().id);
		}

		// Write the function call (final calls are virtual ONLY to the VM)
		g_Object->WriteOp(OPCODE_CALLV, function->GetIndex());

		// Reclaim the allocated stack memory
		if (function->GetNumParams())
			g_Object->WriteOp(OPCODE_ADDST, function->GetNumParams() << 2);
	}

	// If the code is in an assignment, push the return value on the stack for use
	if (flags & FLAGS_ASSIGNMENT)
		g_Object->WriteOp(OPCODE_PUSH_RETURN);

	outfunc;
}


void FunctionCall::Optimise(ParseTreeNode **holder)
{
	infunc(FunctionCall::Optimise);

	int		x;

	// Optimise every expression in the function call
	for (x = 0; x < param_list.GetPosition(); x++)
		param_list(x)->Optimise(NULL);

	outfunc;
}


void FunctionCall::ResolveType(int flags)
{
	infunc(FunctionCall::ResolveType);

	int		x, passed, needed;
	char	name[MAX_LABEL_SIZE];

	// Get the function name
	tokeniser->IsolateString(token, name);

	if (function == NULL)
	{
		// Get the function pointer
		if ((function = g_Env->GetActiveClass()->GetFunction(name, flags)) == NULL)
			if ((function = g_Env->GetActiveClass()->GetImport(name)) == NULL)
				throw CompileError("(Line %d) Undefined label '%s'", token.line, name);
	}

	// Get the number of parameters passed and the number needed
	passed = param_list.GetPosition();
	needed = function->GetNumParams();

	// Error!
	if (passed != needed)
	{
		if (passed < needed)
			throw CompileError("(Line %d) Not enough parameters passed to function '%s'", token.line, name);
		else
			throw CompileError("(Line %d) Too many parameters passed to function '%s'", token.line, name);
	}

	// Get the type from the function return type
	out_type = function->GetReturnType();
	in_type.Make(VARIABLE_TYPEID_VOID);

	// Resolve all types for each parameter
	for (x = 0; x < passed; x++)
	{
		VarType		type_p, type_n;

		// ... Need to check here to see if each parameter matches type ...
		param_list(x)->ResolveType(flags);

		// Get the passed and needed types
		type_p = param_list(x)->out_type;
		type_n = function->GetParameter(x)->GetType();

		// Check stuff if they are not equal
		if (type_n != type_p)
		{
			// Check for illegal array operations
			if (type_n.array != type_p.array)
				throw CompileError("(Line %d) Function parameter %d array confusion", token.line, x + 1);

			// Arrays of the same size must be operated on
			if (type_n.array && type_p.array)
			{
				if (type_n.elements != type_p.elements)
					throw CompileError("(Line %d) Function parameter %d array elements do not match", token.line, x + 1);
			}

			// Are both types custom?
			if (type_n.id == VARIABLE_TYPEID_CUSTOM && type_p.id == VARIABLE_TYPEID_CUSTOM)
			{
				// If the types are not the same
				if (strcmp(type_n.name, type_p.name))
				{
					Class	*class_ptr;

					// This should never be NULL
					class_ptr = g_Env->GetClass(type_p.name);

					if (!class_ptr->HasSuperClass(type_n.name))
						throw CompileError("(Line %d) Function parameter %d incompatible types", token.line, x + 1);
				}
			}

			else
			{
				// Mixed custom/non-custom types
				if (type_n.id == VARIABLE_TYPEID_CUSTOM || type_p.id == VARIABLE_TYPEID_CUSTOM)
					throw CompileError("(Line %d) Function parameter %d incompatible types", token.line, x + 1);

				// Mixed string/non-string types
				if (type_n.id == VARIABLE_TYPEID_STRING || type_p.id == VARIABLE_TYPEID_STRING)
					throw CompileError("(Line %d) Function parameter %d incompatible types", token.line, x + 1);
			}
		}
	}

	outfunc;
}


int FunctionCall::GetNodeType(void)
{
	infunc(FunctionCall::GetNodeType);

	return (PTNODE_TYPE_FUNCTIONCALL);

	outfunc;
}


void FunctionCall::DebugPrint(int indent, char *data)
{
	infunc(FunctionCall::DebugPrint);

	char	token_string[MAX_LABEL_SIZE];
	int		x;

	// Copy the string
	for (x = 0; x < token.length; x++)
		token_string[x] = data[token.position + x];

	// Terminate it
	token_string[x] = 0;

	// Print the indent
	for (x = 0; x < indent; x++)
		printf(" ");

	// Now the token
	printf("%s(\n", token_string);

	// Print each expression
	for (x = 0; x < param_list.GetPosition(); x++)
	{
		param_list(x)->DebugPrint(indent, data);
		if (x != param_list.GetPosition() - 1)
			printf(",\n");
	}

	// Close it
	printf(")\n");

	outfunc;
}