
#include "..\include\main.h"


IDLParser::IDLParser(Tokeniser *_tokeniser, char *_data)
{
	// Set some stuff
	tokeniser = _tokeniser;
	data = _data;
	cur_interface = NULL;
}


void IDLParser::ProcessMethodTerm(int flags)
{
	infunc(IDLParser::ProcessMethodTerm);

	VarType		ret_type;
	char		string[MAX_LABEL_SIZE];
	Function	*method;
	int			x = 0;

	if (!(flags & FLAGS_IN_INTERFACE))
		throw CompileError("(Line %d) Must be in an interface to declare a method", CUR_TOKEN.line);

	// Keyword
	INC_TOKEN;

	// Get the method return type
	GetTypeInfo(ret_type, 0);

	// Grab the array information
	if (CUR_TOKEN.type == TOKEN_ARRAY_OPEN)
		GetArrayInfo(ret_type);

	if (CUR_TOKEN.type != TOKEN_NAME)
		throw CompileError("(Line %d) Expecting a method name", CUR_TOKEN.line);

	// Grab the name
	tokeniser->IsolateString(CUR_TOKEN, string);
	INC_TOKEN;

	// Create the new function
	method = new Function;
	method->SetName(string);
	method->SetReturnType(ret_type);
	method->SetFlag(FFLAGS_IMPORTED);

	// Check for multiple declarations
	if (cur_interface->GetFunction(string, 0))
		throw CompileError("(Line %d) Interface to method '%s' already has a VM code body.", CUR_TOKEN.line, string);

	if (cur_interface->GetImport(string))
		throw CompileError("(Line %d) Interface to method %s has already been declared.", CUR_TOKEN.line, string);

	// Add it to the imported list
	method->SetIndex(cur_interface->NewImportIndex());
	cur_interface->imp_functions.Add(string, method);

	if (CUR_TOKEN.type != TOKEN_LEFT_BRACKET)
		throw CompileError("(Line %d) No opening bracket specifying parameters found.", CUR_TOKEN.line);

	// Left bracket
	INC_TOKEN;

	while (1)
	{
		VarType		type;
		Variable	*var;

		// End of function
		if (CUR_TOKEN.type == TOKEN_RIGHT_BRACKET)
			break;

		if (x && CUR_TOKEN.type != TOKEN_COMMA)
			throw CompileError("(Line %d) Expecting comma seperator for parameter list.", CUR_TOKEN.line);

		// Comma
		if (x) INC_TOKEN;

		if (CUR_TOKEN.type == TOKEN_VOID)
		{
			// Can't have void types, they must be singular
			if (x)
				throw CompileError("(Line %d) Illegal positioning of 'void' type at parameter %d", CUR_TOKEN.line, x + 1);

			// Leave the loop
			INC_TOKEN;
			break;
		}

		// Get type info for the current parameter
		GetTypeInfo(type, 0);

		if (CUR_TOKEN.type != TOKEN_NAME)
			throw CompileError("(Line %d) Expecting the name of parameter %d.", CUR_TOKEN.line, x + 1);

		// Grab the parameter name
		tokeniser->IsolateString(CUR_TOKEN, string);
		INC_TOKEN;

		// Check for the array component
		if (CUR_TOKEN.type == TOKEN_ARRAY_OPEN)
			GetArrayInfo(type);

		// Add the parameter to the method!
		var = method->AddParameter(type, string);

		x++;
	}

	if (CUR_TOKEN.type != TOKEN_RIGHT_BRACKET)
		throw CompileError("(Line %d) No closing bracket specified", CUR_TOKEN.line);

	// Right bracket
	INC_TOKEN;

	if (CUR_TOKEN.type != TOKEN_END_OF_LINE)
		throw CompileError("(Line %d) Expecting end of line", CUR_TOKEN.line);

	outfunc;
}


void IDLParser::ProcessInterfaceTerm(int flags)
{
	infunc(IDLParser::ProcessInterfaceTerm);

	char	string[MAX_LABEL_SIZE];
	int		created = 0;

	// Keyword
	INC_TOKEN;

	// Grab the name
	tokeniser->IsolateString(CUR_TOKEN, string);

	// See if an local side implementation of the interface exists
	if ((cur_interface = g_Env->GetClass(string)) == NULL)
	{
		// Else create one
		cur_interface = new Class;

		// Set the name
		cur_interface->SetName(string);

		// This is a pure interface, with no VM thread
		cur_interface->SetFlag(CLASS_FLAGS_INTERFACE);

		created = 1;
	}

	// Name
	INC_TOKEN;

	// Does this interface extend another one?
	if (CUR_TOKEN.type == TOKEN_EXTENDS)
	{
		// Extends
		INC_TOKEN;

		if (CUR_TOKEN.type != TOKEN_NAME)
			throw CompileError("(Line %d) Expecting the name of an interface to extend", CUR_TOKEN.line);

		// Grab the name
		tokeniser->IsolateString(CUR_TOKEN, string);

		// Either set the super-class or compare it with that of the vm-side implementation
		if (created)
			cur_interface->SetSuperClass(string);
		else
			if (strcmp(cur_interface->GetSuperClass(), string))
				throw CompileError("(Line %d) Interface and VM-side implementation super-classes do not match", CUR_TOKEN.line);

		// Name
		INC_TOKEN;
	}

	ProcessBlock(flags | FLAGS_IN_INTERFACE);

	// Close block
	INC_TOKEN;

	// Add the pure interface to the environment
	if (created)
	{
		cur_interface->SetDefined();
		g_Env->AddClassPtr(cur_interface);
	}

	cur_interface = NULL;

	outfunc;
}


void IDLParser::ProcessKeywordTerm(int flags)
{
	infunc(IDLParser::ProcessKeywordTerm);

	// Differentiate by type
	switch (CUR_TOKEN.type)
	{
		case (TOKEN_INTERFACE):
			ProcessInterfaceTerm(flags);
			break;

		case (TOKEN_METHOD):
			ProcessMethodTerm(flags);
			break;
	}

	outfunc;
}


void IDLParser::ProcessBlock(int flags)
{
	infunc(IDLParser::ProcessBlock);

	// Check for an opening block
	if (CUR_TOKEN.type != TOKEN_BLOCK_OPEN)
		throw CompileError("(Line %d) Expecting an opening block", CUR_TOKEN.line);

	// Block open
	INC_TOKEN;

	while (1)
	{
		// Break if the end of the block has been reached
		if (CUR_TOKEN.type == TOKEN_BLOCK_CLOSE)
			break;

		ProcessStatement(flags);
	}

	outfunc;
}


void IDLParser::ProcessTerm(int flags)
{
	infunc(IDLParser::ProcessTerm);

	// Differentiate by group
	switch (CUR_TOKEN.Group())
	{
		case (TOKEN_GROUP_KEYWORD):
			ProcessKeywordTerm(flags);
			break;

		default:
			throw CompileError("(Line %d) Illegal token", CUR_TOKEN.line);
			break;
	}

	outfunc;
}


void IDLParser::ProcessStatement(int flags)
{
	infunc(IDLParser::ProcessStatement);

	// This term should be enough to launch a statement of its own
	ProcessTerm(flags);
	INC_TOKEN;

	outfunc;
}


void IDLParser::Begin(void)
{
	infunc(IDLParser::Begin);

	// Being processing the aye dee el!

	while (1)
	{
		// Start processing
		ProcessTerm(0);

		// If reached the end of the file, break
		if (CUR_TOKEN.type == TOKEN_NULL)
			break;
	}

	outfunc;
}


void IDLParser::GetTypeInfo(VarType &type, int standard_types)
{
	infunc(IDLParser::GetTypeInfo);

	char	name[MAX_LABEL_SIZE];

	switch (CUR_TOKEN.type)
	{
		// These are the standard types that can be mixed with signed/unsigned

		case (TOKEN_DECLARE_INT):
			type.Make(VARIABLE_TYPEID_INT);
			INC_TOKEN;
			break;

		case (TOKEN_DECLARE_SHORT):
			type.Make(VARIABLE_TYPEID_SHORT);
			INC_TOKEN;
			break;

		case (TOKEN_DECLARE_CHAR):
			type.Make(VARIABLE_TYPEID_CHAR);
			INC_TOKEN;
			break;

		// For signed/unsigned, call the function again for the next token but request
		// standard types.

		case (TOKEN_DECLARE_SIGNED):
			INC_TOKEN;
			GetTypeInfo(type, 1);
			break;

		case (TOKEN_DECLARE_UNSIGNED):
			INC_TOKEN;
			GetTypeInfo(type, 1);

			type.id |= VARIABLE_TYPEID_UNSIGNED;
			sprintf(name, "unsigned %s", type.name);
			strcpy(type.name, name);
			break;

		// These types cannot be mixed with signed/unsigned so flag errors if they are

		case (TOKEN_VOID):
			if (standard_types)
				throw CompileError("(Line %d) Cannot combine 'void' with signed/unsigned", CUR_TOKEN.line);

			type.Make(VARIABLE_TYPEID_VOID);
			INC_TOKEN;
			break;

		case (TOKEN_DECLARE_FLOAT):
			if (standard_types)
				throw CompileError("(Line %d) Cannot combine 'float' with signed/unsigned", CUR_TOKEN.line);

			type.Make(VARIABLE_TYPEID_FLOAT);
			INC_TOKEN;
			break;

		case (TOKEN_DECLARE_STRING):
			if (standard_types)
				throw CompileError("(Line %d) Cannot combine 'string' with signed/unsigned", CUR_TOKEN.line);

			type.Make(VARIABLE_TYPEID_STRING);
			INC_TOKEN;
			break;

		// This is a user-defined type, set it to custom and copy the name

		default:
			if (standard_types)
				throw CompileError("(Line %d) Cannot combine type with signed/unsigned", CUR_TOKEN.line);

			type.id = VARIABLE_TYPEID_CUSTOM;
			tokeniser->IsolateString(CUR_TOKEN, name);
			strcpy(type.name, name);
			INC_TOKEN;
			break;
	}

	// Not an array by default
	type.array = 0;
	type.elements = 0;

	outfunc;
}


void IDLParser::GetArrayInfo(VarType &type)
{
	infunc(IDLParser::GetArrayInfo);

	char	string[MAX_LABEL_SIZE];

	// Array open
	INC_TOKEN;

	if (CUR_TOKEN.type != TOKEN_NUMBER)
		throw CompileError("(Line %d) Expecting a number for the array dimension.", CUR_TOKEN.line);

	// Setup the return type as an array
	tokeniser->IsolateString(CUR_TOKEN, string);
	type.array = 1;
	type.elements = atoi(string);

	// Number
	INC_TOKEN;

	if (CUR_TOKEN.type != TOKEN_ARRAY_CLOSE)
		throw CompileError("(Line %d) Expecting the array to close.", CUR_TOKEN.line);

	// Array close
	INC_TOKEN;

	outfunc;
}