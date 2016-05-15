
#include "..\include\main.h"


CodeGenerator::CodeGenerator(Tokeniser *_tokeniser, char *_data)
{
	// Set some stuff
	tokeniser = _tokeniser;
	data = _data;
	cur_class = NULL;
}


CodeGenerator::~CodeGenerator(void)
{
}


void CodeGenerator::ProcessParseTree(int flags)
{
	infunc(CodeGenerator::ProcessParseTree);

	ParseTree	*tree;

	// Allocate some space for the parse tree
	if ((tree = new ParseTree) == NULL)
		throw CError("Couldn't allocate parse tree");

	// Build the parse tree (steps passed all the tokens)
	tree->Build(tokeniser, TOKEN_END_OF_LINE, TOKEN_NULL);

	// Reduce the tree if possible
	tree->Optimise();

	if (g_Object)
	{
		tree->CompleteTypes(flags);

		tree->GenerateCode(flags);

		// Yacka
//		tree->Debug();
	}

	// The tree is no longer needed
	delete tree;

	outfunc;
}


void CodeGenerator::IsolateTokenString(Token &token)
{
	int		x;

	infunc(CodeGenerator::IsolateTokenString);

	// Copy the string
	for (x = 0; x < token.length; x++)
		token_string[x] = data[token.position + x];

	// Terminate it
	token_string[x] = 0;

	outfunc;
}


void CodeGenerator::ProcessClassModifiers(Class *class_ptr)
{
	infunc(CodeGenerator::ProcessClassModifiers);

	int		loop = 1;

	// Loop for a while
	while (loop)
	{
		switch (CUR_TOKEN.type)
		{
			// Does this class inherit?
			case (TOKEN_EXTENDS):
				INC_TOKEN;

				// Need a name for the super-class
				if (CUR_TOKEN.type != TOKEN_NAME)
					throw CompileError("(Line %d) Expecting name of super-class", CUR_TOKEN.line);

				// Grab the name
				IsolateTokenString(CUR_TOKEN);
				INC_TOKEN;

				// Set the super-class!
				if (g_Object == NULL)
					class_ptr->SetSuperClass(token_string);
				break;


			// Is this an abstract class? (Can't be instantiated)
			case (TOKEN_ABSTRACT):
				INC_TOKEN;

				// Set the abstract flag
				if (g_Object == NULL)
					class_ptr->SetFlag(CLASS_FLAGS_ABSTRACT);
				break;


			// Break out of the keyword search
			case (TOKEN_BLOCK_OPEN):
				loop = 0;
				break;


			// Anything else is an error
			default:
				throw CompileError("(Line %d) Illegal token after class definition", CUR_TOKEN.line);
				break;
		}
	}

	outfunc;
}


void CodeGenerator::ProcessClassTerm(int flags)
{
	Class	*class_ptr;

	infunc(CodeGenerator::ProcessClassTerm);

	if (flags & FLAGS_IN_CLASS)
		throw CompileError("(Line %d) Already within a class", CUR_TOKEN.line);

	if (NEXT_TOKEN.type != TOKEN_NAME)
		throw CompileError("(Line %d) Expecting class name", CUR_TOKEN.line);

	// Move onto the name and grab it
	INC_TOKEN;
	IsolateTokenString(CUR_TOKEN);

	// Sneak passed it
	INC_TOKEN;

	if (g_Object == NULL)
	{
		// Allocate the memory
		if ((class_ptr = new Class) == NULL)
			throw CError("Couldn't allocate Class structure");
		class_ptr->SetName(token_string);
	}
	else
	{
		// Class already defined, get it from the environment
		class_ptr = g_Env->GetClass(token_string);
	}

	ProcessClassModifiers(class_ptr);

	// Set the current class
	g_Env->SetActiveClass(class_ptr);
	cur_class = class_ptr;

	ProcessBlock(flags | FLAGS_IN_CLASS);

	// Go passed the close block
	INC_TOKEN;

	if (g_Object == NULL)
	{
		// Add the defined class to the environment
		class_ptr->SetDefined();
		g_Env->AddClassPtr(class_ptr);
	}
	else
	{
		// Write the class information to file
		g_Object->WriteClassInfo(cur_class);
	}

	outfunc;
}


void CodeGenerator::ProcessVarTerm(int flags)
{
	infunc(CodeGenerator::ProcessVarTerm);

	char	name[MAX_LABEL_SIZE];
	VarType	type;

	if ((flags & FLAGS_IN_STATE) && !(flags & FLAGS_IN_FUNCTION))
		throw CompileError("(Line %d) Not allowed variable declarations inside states, but outside of functions", CUR_TOKEN.line);

	// Grab the type
	GetTypeInfo(type, 0);

	// Loop until the declaration has ended
	while (CUR_TOKEN.type != TOKEN_END_OF_LINE)
	{
		Variable	*var;

		if (CUR_TOKEN.type != TOKEN_NAME)
			throw CompileError("(Line %d) No name after type declaration", CUR_TOKEN.line);

		// Get the current variable name
		IsolateTokenString(CUR_TOKEN);
		strcpy(name, token_string);
		INC_TOKEN;

		// Grab the array dimensions if it's an array
		if (CUR_TOKEN.type == TOKEN_ARRAY_OPEN)
			GetArrayInfo(type);

		if (g_Object == NULL)
		{
			if (flags & FLAGS_IN_FUNCTION)
			{
				var = cur_class->cur_function->AddLocal(type, name);
			}
			else
			{
				// Create a new variable
				if ((var = new Variable) == NULL)
					throw CError("Couldn't allocate variable structure");

				var->SetType(type);
				var->SetScope(VARIABLE_SCOPE_MEMBER);

				if (type.array)
				{
					var->SetAddress(cur_class->NewAddress(4 + (type.elements << 2), 0));
					var->SetArrayElements(type.elements);
				}
				else
					var->SetAddress(cur_class->NewAddress(4, 1));

				// Add it to the class list of variables
				cur_class->variables.Add(name, var);
			}
		}
		else
		{
			// Variable already defined, get it from the class
			var = cur_class->GetVariable(name, flags);

			// Check for invalid custom variable types
			if (var->GetType().id == VARIABLE_TYPEID_CUSTOM)
				if (g_Env->GetClass(var->GetType().name) == NULL)
					throw CompileError("(Line %d) Undefined variable type", CUR_TOKEN.line);

			// Local variables that are arrays need to be prepared
			if (flags & FLAGS_IN_FUNCTION && var->GetType().array)
				g_Object->WriteOp(OPCODE_ARRAYP, var->GetAddress());
		}

		// Check for initial values
		if (CUR_TOKEN.type == TOKEN_ASSIGN)
		{
			if (type.id == VARIABLE_TYPEID_CUSTOM)
				throw CompileError("(Line %d) Not allowed immediate assignments on custom variable types", CUR_TOKEN.line);

			INC_TOKEN;

			// Is this an array?
			if (var->GetArrayElements())
			{
				int		x = 0, neg;
				dynamic	num;

				neg = 0;

				if (CUR_TOKEN.type != TOKEN_BLOCK_OPEN)
					throw CompileError("(Line %d) Expecting block open for array default value definition", CUR_TOKEN.line);

				INC_TOKEN;

				while (1)
				{
					// Check for exit
					if (CUR_TOKEN.type == TOKEN_BLOCK_CLOSE)
					{
						INC_TOKEN;
						break;
					}

					// Check for too many variables
					if (x == var->GetArrayElements())
						throw CompileError("(Line %d) Too many initialisers for array", CUR_TOKEN.line);

					// Read the value
					ReadConstant(&num, type, TOKEN_COMMA, TOKEN_BLOCK_CLOSE);
					if (g_Object == NULL)
						var->SetElementValue(x, &num);

					x++;

					// Check for the comma
					if (CUR_TOKEN.type != TOKEN_BLOCK_CLOSE)
					{
						if (CUR_TOKEN.type != TOKEN_COMMA)
							throw CompileError("(Line %d) Expecting comma seperator", CUR_TOKEN.line);

						INC_TOKEN;

						// Check for situations like = { 4, };
						if (CUR_TOKEN.type == TOKEN_BLOCK_CLOSE)
							throw CompileError("(Line %d) Expecting array element", CUR_TOKEN.line);
					}
				}
			}

			// No
			else
			{
				dynamic	num;

				ReadConstant(&num, type, TOKEN_END_OF_LINE, TOKEN_COMMA);
				if (g_Object == NULL)
					var->SetDefaultValue(&num);
			}
		}

		// Check for multiple variable declarations
		if (CUR_TOKEN.type == TOKEN_COMMA)
		{
			INC_TOKEN;

			if (CUR_TOKEN.type == TOKEN_END_OF_LINE)
				throw CompileError("(Line %d) Unexpected end of line", CUR_TOKEN.line);
		}
		else if (CUR_TOKEN.type != TOKEN_END_OF_LINE)
			throw CompileError("(Line %d) Illegal token after variable declaration", CUR_TOKEN.line);
	}

	outfunc;
}


CodeGenerator::BlockType CodeGenerator::ProcessBlock(int flags)
{
	infunc(CodeGenerator::ProcessBlock);

	// Check for a mandatory opening block
	if (!(flags & FLAGS_ALLOW_LINE_BLOCKS) && ((flags & FLAGS_IN_STATE) || (flags & FLAGS_IN_CLASS) || (flags & FLAGS_IN_FUNCTION)))
		if (CUR_TOKEN.type != TOKEN_BLOCK_OPEN)
			throw CompileError("(Line %d) Expecting an opening block", CUR_TOKEN.line);

	// Reduce the lifetime
	flags &= ~FLAGS_ALLOW_LINE_BLOCKS;

	// Determine how to read the block
	if (CUR_TOKEN.type == TOKEN_BLOCK_OPEN)
	{
		INC_TOKEN;

		while (1)
		{
			// Break if the end of the block has been reached
			if (CUR_TOKEN.type == TOKEN_BLOCK_CLOSE)
				break;

			ProcessStatement(flags);
		}

		return (BT_SCOPED);
	}
	else
	{
		ProcessStatement(flags | FLAGS_IGNORE_END_TOKEN);
	}

	return (BT_STATEMENT);

	outfunc;
}


void CodeGenerator::ProcessIfTerm(int flags)
{
	infunc(CodeGenerator::ProcessIfTerm);

	ParseTree		*tree;
	ParseTreeNode	*top;
	int				a, b;

	a = b = 100 + rand();
	while (a == b) b = 100 + rand();

	if (!(flags & FLAGS_IN_CLASS))
		throw CompileError("(Line %d) Can't have an if statement outside of a class", CUR_TOKEN.line);

	// Go passed the declare
	INC_TOKEN;

	if (CUR_TOKEN.type != TOKEN_LEFT_BRACKET)
		throw CompileError("(Line %d) Expecting an opening bracket", CUR_TOKEN.line);

	// Should be at the expression opening here
	INC_TOKEN;

	// Allocate the parse tree for the expression
	if ((tree = new ParseTree) == NULL)
		throw CError("Couldn't allocate parse tree");

	// Build the tree, ending at the if close
	tree->Build(tokeniser, TOKEN_RIGHT_BRACKET, TOKEN_NULL);
	INC_TOKEN;

	// Reduce the tree
	tree->Optimise();

	if (g_Object)
	{
		tree->CompleteTypes(flags | FLAGS_IMPLICIT_ASSIGNMENT);

		// Implicit assignment (no need for asg at the end of the expression)
		tree->GenerateCode(flags | FLAGS_IMPLICIT_ASSIGNMENT);
	}

	if (tree->GetTop() == NULL)
		throw CompileError("(Line %d) Expecting an expression for the if statement", CUR_TOKEN.line);

	if (g_Object)
	{
		// Mark this jump
		g_Object->AddBackpatchItem(a);

		// Grab the top tree node
		if ((top = tree->GetTop()) == NULL)
			throw CompileError("(Line %d) Expecting an expression for the if statement", CUR_TOKEN.line);

		// Jump over the following block if the expression is false
		if (top->out_type.id == VARIABLE_TYPEID_FLOAT)
			g_Object->WriteOp(OPCODE_FJZ, 0);
		else
			g_Object->WriteOp(OPCODE_JZ, 0);
	}

	// The tree is no longer needed
	delete tree;

	BlockType bt = ProcessBlock(flags | FLAGS_ALLOW_LINE_BLOCKS);

	if (NEXT_TOKEN.type == TOKEN_ELSE)
	{
		// Step over the close scope
		INC_TOKEN;

		if (g_Object)
		{
			// Mark this jump, at the end of the if's true block
			g_Object->AddBackpatchItem(b);

			// Jump over the else false block
			g_Object->WriteOp(OPCODE_JMP, 0);
		}
	}

	if (g_Object)
		g_Object->UpdateItems(a);

	if (CUR_TOKEN.type == TOKEN_ELSE)
	{
		// Step over the keyword
		INC_TOKEN;

		ProcessBlock(flags | FLAGS_ALLOW_LINE_BLOCKS);

		// Back patch the jump over the else block
		if (g_Object)
			g_Object->UpdateItems(b);
	}

	outfunc;
}


void CodeGenerator::ProcessForTerm(int flags)
{
	infunc(CodeGenerator::ProcessForTerm);

	ParseTree	*tree, *expression;
	int			code_position;

	if (!(flags & FLAGS_IN_CLASS))
		throw CompileError("(Line %d) Can't have a for statement outside of a class", CUR_TOKEN.line);

	// Go passed the declare
	INC_TOKEN;

	if (CUR_TOKEN.type != TOKEN_LEFT_BRACKET)
		throw CompileError("(Line %d) Expecting an opening bracket", CUR_TOKEN.line);

	// Should be at the expression opening here
	INC_TOKEN;

	// Allocate the parse tree for the first expression
	if ((tree = new ParseTree) == NULL)
		throw CError("Couldn't allocate parse tree");

	// Build the tree, ending at the first semi-colon
	tree->Build(tokeniser, TOKEN_END_OF_LINE, TOKEN_NULL);
	INC_TOKEN;

	// Reduce the tree
	tree->Optimise();

	// Generate the intialisation statement
	if (g_Object)
	{
		tree->CompleteTypes(flags);
		tree->GenerateCode(flags);
	}

	// Don't need this anymore
	delete tree;

	// Allocate the parse tree for the expression part of the for
	if ((expression = new ParseTree) == NULL)
		throw CError("Couldn't allocate parse tree");

	// Build the tree, ending at the second semi-colon
	expression->Build(tokeniser, TOKEN_END_OF_LINE, TOKEN_NULL);
	INC_TOKEN;

	// Reduce the tree
	expression->Optimise();

	// Allocate the parse tree for the loop execution part of the for
	if ((tree = new ParseTree) == NULL)
		throw CError("Couldn't allocate parse tree");

	// Build the tree, ending at the closing bracket
	tree->Build(tokeniser, TOKEN_RIGHT_BRACKET, TOKEN_NULL);
	INC_TOKEN;

	// Mark the beginning of the for loop
	if (g_Object)
		code_position = g_Object->GetPosition();

	ProcessBlock(flags | FLAGS_ALLOW_LINE_BLOCKS | FLAGS_ALLOW_BREAK_CONTINUE);

	if (g_Object)
	{
		ParseTreeNode *top;

		// Update all continues
		g_Object->UpdateItems(13);

		// Generate the code the executes at the end of each for-loop
		tree->CompleteTypes(flags);
		tree->GenerateCode(flags);

		// Grab the top tree node
		if ((top = expression->GetTop()) == NULL)
		{
			// If no test expression is given, jump indefinitely to the end of the loop
			g_Object->WriteOp(OPCODE_JMP, code_position);
		}

		else
		{
			// Generate the expression code
			expression->CompleteTypes(flags | FLAGS_IMPLICIT_ASSIGNMENT);
			expression->GenerateCode(flags | FLAGS_IMPLICIT_ASSIGNMENT);

			// Loop to the beginning if the expression is true
			if (top->out_type.id == VARIABLE_TYPEID_FLOAT)
				g_Object->WriteOp(OPCODE_FJNZ, code_position);
			else
				g_Object->WriteOp(OPCODE_JNZ, code_position);
		}

		// Update all breaks
		g_Object->UpdateItems(12);
	}

	// Both trees no longer needed
	delete tree;
	delete expression;

	outfunc;
}


void CodeGenerator::ProcessBreakTerm(int flags)
{
	infunc(CodeGenerator::ProcessBreakTerm);
	
	if (!(flags & FLAGS_ALLOW_BREAK))
		throw CompileError("(Line %d) Illegal use of break keyword");

	// Go over the keyword
	INC_TOKEN;

	if (g_Object)
	{
		// Add the break backpatch item
		g_Object->AddBackpatchItem(12);

		// Jump out of the loop
		g_Object->WriteOp(OPCODE_JMP, 0);
	}

	outfunc;
}


void CodeGenerator::ProcessContinueTerm(int flags)
{
	infunc(CodeGenerator::ProcessContinueTerm);

	if (!(flags & FLAGS_ALLOW_CONTINUE))
		throw CompileError("(Line %d) Illegal use of continue keyword");

	// Go over the keyword
	INC_TOKEN;

	if (g_Object)
	{
		// Add the continue backpatch item
		g_Object->AddBackpatchItem(13);

		// Jump to the beginning of the loop
		g_Object->WriteOp(OPCODE_JMP, 0);
	}

	outfunc;
}


void CodeGenerator::ProcessWhileTerm(int flags)
{
	infunc(CodeGenerator::ProcessWhileTerm);

	ParseTree	*tree;
	int			code_position;

	if (!(flags & FLAGS_IN_CLASS))
		throw CompileError("(Line %d) Can't have a while loop statement outside of a class", CUR_TOKEN.line);

	// Go passed the declare
	INC_TOKEN;

	if (CUR_TOKEN.type != TOKEN_LEFT_BRACKET)
		throw CompileError("(Line %d) Expecting an opening bracket", CUR_TOKEN.line);

	// Should be at the expression opening here
	INC_TOKEN;

	// Allocate the parse tree for the test expression
	if ((tree = new ParseTree) == NULL)
		throw CError("Couldn't allocate parse tree");

	// Build the tree, ending at the closing bracket
	tree->Build(tokeniser, TOKEN_RIGHT_BRACKET, TOKEN_NULL);
	INC_TOKEN;

	// Reduce the tree
	tree->Optimise();

	if (g_Object)
	{
		// Mark before the loop test
		code_position = g_Object->GetPosition();

		// Generate the test statement
		tree->CompleteTypes(flags | FLAGS_IMPLICIT_ASSIGNMENT);
		tree->GenerateCode(flags | FLAGS_IMPLICIT_ASSIGNMENT);
	}

	// Valid?
	if (tree->GetTop() == NULL)
		throw CompileError("(Line %d) Expecting an expression for the if statement", CUR_TOKEN.line);

	if (g_Object)
	{
		g_Object->AddBackpatchItem(14);

		// Generate the jump out of the loop
		if (tree->GetTop()->out_type.id == VARIABLE_TYPEID_FLOAT)
			g_Object->WriteOp(OPCODE_FJZ, 0);
		else
			g_Object->WriteOp(OPCODE_JZ, 0);
	}

	// Don't need this anymore
	delete tree;

	ProcessBlock(flags | FLAGS_ALLOW_LINE_BLOCKS | FLAGS_ALLOW_BREAK_CONTINUE);

	if (g_Object)
	{
		// Jump back to the beginning of the loop
		g_Object->WriteOp(OPCODE_JMP, code_position);

		// Update the loop jump out
		g_Object->UpdateItems(14);

		// Update all breaks
		g_Object->UpdateItems(12);

		// Update all continues
		g_Object->UpdateItems(13, code_position);
	}

	outfunc;
}


void CodeGenerator::ProcessStateTerm(int flags)
{
	State	*state_ptr;

	infunc(CodeGenerator::ProcessStateTerm);

	if (!(flags & FLAGS_IN_CLASS))
		throw CompileError("(Line %d) Can't declare states outside of a class", CUR_TOKEN.line);

	if (flags & FLAGS_IN_STATE)
		throw CompileError("(Line %d) Nested states are not allowed", CUR_TOKEN.line);

	// Go passed the declare
	INC_TOKEN;
	
	if (CUR_TOKEN.type != TOKEN_NAME)
		throw CompileError("(Line %d) No name provided for the state", CUR_TOKEN.line);

	// Grab the state name
	IsolateTokenString(CUR_TOKEN);
	INC_TOKEN;

	if (g_Object == NULL)
	{
		// Allocate some space for the state
		if ((state_ptr = new State) == NULL)
			throw CError("Couldn't allocate state structure memory");

		// Setup the state parameters
		state_ptr->SetIndex(cur_class->NewStateIndex());

		// Add the state to the list
		cur_class->states.Add(token_string, state_ptr);
	}
	else
	{
		// The state is already defined, get it from the class
		state_ptr = cur_class->GetState(token_string);

		// Valid to set the position now
		state_ptr->SetPosition(g_Object->GetPosition());
	}

	cur_class->cur_state = state_ptr;

	ProcessBlock(flags | FLAGS_IN_STATE);

	if (g_Object)
	{
		Class	*class_ptr = cur_class;
		int		found = 0;
		char	state_name[MAX_LABEL_SIZE];

		// End the state (suspends the thread)
		g_Object->WriteOp(OPCODE_ENDSTATE);

		strcpy(state_name, state_ptr->GetName());

		// Search for a 'begin' label for this state
		while (class_ptr)
		{
			// If the state exists in this class, see if a start is specified
			if (state_ptr && state_ptr->GetCodeStart() != -1)
			{
				found = 1;
				break;
			}

			// Get the super-class
			class_ptr = class_ptr->sclass_ptr;

			// See if the state exists in this class
			if (class_ptr)
				state_ptr = class_ptr->GetState(state_name);
		}

		if (!found)
			throw CompileError("(Line %d) No 'begin' label specified for state '%s'", CUR_TOKEN.line, state_name);
	}

	outfunc;
}


void CodeGenerator::ProcessFunctionTerm(int flags)
{
	VarType		ret_type;
	Function	*func_ptr;
	int			final = 0;

	infunc(CodeGenerator::ProcessFunctionTerm);

	INC_TOKEN;

	// Check for function modifiers
	switch (CUR_TOKEN.type)
	{
		// Final definition of this function within the hierarchy
		case (TOKEN_FINAL):
			ProcessFunctionTerm(flags | FLAGS_FUNCTION_FINAL);
			return;

		// This function is externally defined (C++)
		case (TOKEN_IMPORT):
			ProcessFunctionTerm(flags | FLAGS_FUNCTION_IMPORT);
			return;
	}

	// Does this specify a return type?
	if (CUR_TOKEN.Group() == TOKEN_GROUP_KEYWORD)
	{
		GetTypeInfo(ret_type, 0);

		// Check for invalid variable return types on the 2nd pass
		if (g_Object)
			if (ret_type.id == VARIABLE_TYPEID_CUSTOM)
				if (g_Env->GetClass(ret_type.name) == NULL)
					throw CompileError("(Line %d) Undefined variable type", CUR_TOKEN.type);

		// Process the array dimensions if it returns an array
		if (CUR_TOKEN.type == TOKEN_ARRAY_OPEN)
		{
			GetArrayInfo(ret_type);
		}
	}

	// No return specified, treat as void
	else if (CUR_TOKEN.type == TOKEN_NAME)
	{
		ret_type.Make(VARIABLE_TYPEID_VOID);
	}

	// Error
	else
	{
		throw CompileError("(Line %d) Illegal token after function declaration", CUR_TOKEN.line);
	}

	if (CUR_TOKEN.type != TOKEN_NAME)
		throw CompileError("(Line %d) No name found for function", CUR_TOKEN.line);

	// Grab the name
	IsolateTokenString(CUR_TOKEN);
	INC_TOKEN;

	if (CUR_TOKEN.type != TOKEN_LEFT_BRACKET)
		throw CompileError("(Line %d) No opening bracket found", CUR_TOKEN.line);

	// Move over the open bracket
	INC_TOKEN;

	if (g_Object == NULL)
	{
		// Create a new function
		if ((func_ptr = new Function) == NULL)
			throw CError("Couldn't allocate function structure");

		// This is an imported function
		if (flags & FLAGS_FUNCTION_IMPORT)
		{
			// Add it to the imported list
			cur_class->imp_functions.Add(token_string, func_ptr);

			// Set function information
			func_ptr->SetIndex(cur_class->NewImportIndex());
			func_ptr->SetFlag(FFLAGS_IMPORTED);
			func_ptr->SetReturnType(ret_type);
		}

		// This is a normal function
		else
		{
			// Add the function to its appropriate list
			if (flags & FLAGS_IN_STATE)
				cur_class->cur_state->functions.Add(token_string, func_ptr);
			else
				cur_class->functions.Add(token_string, func_ptr);

			// Set function information
			func_ptr->SetIndex(cur_class->NewFunctionIndex());
			func_ptr->SetFlag((flags & FLAGS_FUNCTION_FINAL) ? FFLAGS_FINAL : 0);
			func_ptr->SetReturnType(ret_type);

			// Check to see if this is a constructor definition and set the entry point
			if (!strcmp(token_string, cur_class->GetName()))
				cur_class->SetEntryPoint(func_ptr->GetIndex());
		}
	}
	else
	{
		// Imported function
		if (flags & FLAGS_FUNCTION_IMPORT)
		{
			// Function already defined, get it from within the class
			func_ptr = cur_class->GetImport(token_string);
		}

		// Normal function
		else
		{
			// Function already defined, get it from within the class
			func_ptr = cur_class->GetFunction(token_string, flags);

			// Safe to set the code location
			func_ptr->SetCodeLocation(g_Object->GetPosition());

			// Enter stack frame
			if (func_ptr->GetCurAddress() || func_ptr->GetCurPAddress())
				g_Object->WriteOp(OPCODE_INSTF);

			// Allocate some space on the stack
			if (func_ptr->GetCurAddress())
				g_Object->WriteOp(OPCODE_SUBST, func_ptr->GetCurAddress());
		}
	}

	while (1)
	{
		VarType		type;
		Variable	*var;

		// Check for no parameters
		if (CUR_TOKEN.type == TOKEN_VOID)
		{
			// void
			INC_TOKEN;

			if (func_ptr->GetNumParams())
				throw CompileError("(Line %d) Illegal placement of void type", CUR_TOKEN.line);

			if (CUR_TOKEN.type != TOKEN_RIGHT_BRACKET)
				throw CompileError("(Line %d) Expecting closing bracket", CUR_TOKEN.line);

			// Closing bracket
			INC_TOKEN;

			break;
		}

		// That's when to end!
		if (CUR_TOKEN.type == TOKEN_RIGHT_BRACKET)
		{
			INC_TOKEN;
			break;
		}

		// Get the variable type
		GetTypeInfo(type, 0);

		// Check for invalid variable types on the 2nd pass
		if (g_Object && type.id == VARIABLE_TYPEID_CUSTOM)
			if (g_Env->GetClass(type.name) == NULL)
				throw CompileError("(Line %d) Undefined variable type", CUR_TOKEN.line);

		if (CUR_TOKEN.type != TOKEN_NAME)
			throw CompileError("(Line %d) Expecting name", CUR_TOKEN.line);

		// And the name
		IsolateTokenString(CUR_TOKEN);
		INC_TOKEN;

		// If it's an array, determine the dimensions
		if (CUR_TOKEN.type == TOKEN_ARRAY_OPEN)
			GetArrayInfo(type);

		// Add the parameter
		if (g_Object == NULL)
			var = func_ptr->AddParameter(type, token_string);

		if (CUR_TOKEN.type == TOKEN_RIGHT_BRACKET)
		{
			INC_TOKEN;
			break;
		}

		if (CUR_TOKEN.type != TOKEN_COMMA)
			throw CompileError("(Line %d) Need comma to seperate parameters", CUR_TOKEN.line);

		// Comma
		INC_TOKEN;
	}

	// Imported functions
	if (flags & FLAGS_FUNCTION_IMPORT)
	{
		if (g_Object == NULL)
			func_ptr->SingulariseAddresses();

		if (CUR_TOKEN.type != TOKEN_END_OF_LINE)
			throw CompileError("(Line %d) Expecting end of line", CUR_TOKEN.line);
	}

	// Normal functions
	else
	{
		if (g_Object == NULL)
			func_ptr->FixAddresses();

		cur_class->cur_function = func_ptr;

		ProcessBlock(flags | FLAGS_IN_FUNCTION);

		// Simply write the return code for void functions
		if (func_ptr->GetReturnType().id == VARIABLE_TYPEID_VOID)
		{
			if (g_Object)
				func_ptr->WriteReturn(g_Object);
		}
		else
		{
			// If a return type is needed, throw an error if one isn't found last
			if (!func_ptr->had_return)
				throw CompileError("(Line %d) Missing return value for function '%s'", CUR_TOKEN.line, func_ptr->GetName());
		}
	}

	outfunc;
}


void CodeGenerator::ProcessReturnTerm(int flags)
{
	infunc(CodeGenerator::ProcessKeywordTerm);

	ParseTree	*tree;

	if (!(flags & FLAGS_IN_FUNCTION))
		throw CompileError("(Line %d) Cannot specify return keyword outside of a function", CUR_TOKEN.line);

	INC_TOKEN;

	// Does this function return any values?
	if (cur_class->cur_function->GetReturnType().id == VARIABLE_TYPEID_VOID)
	{
		// Can only end here
		if (CUR_TOKEN.type != TOKEN_END_OF_LINE)
			throw CompileError("(Line %d) Cannot specify return value for void function", CUR_TOKEN.line);

		return;
	}

	// Allocate the parse tree
	if ((tree = new ParseTree) == NULL)
		throw CError("Couldn't allocate parse tree");

	// Build the parse tree
	tree->Build(tokeniser, TOKEN_END_OF_LINE, TOKEN_NULL);

	// Reduce it
	tree->Optimise();

	if (g_Object)
	{
		tree->CompleteTypes(flags | FLAGS_IMPLICIT_ASSIGNMENT);

		tree->GenerateCode(flags | FLAGS_IMPLICIT_ASSIGNMENT);
	}

	// Don't need the tree
	delete tree;

	// Mark the return
	cur_class->cur_function->had_return = 1;

	if (g_Object)
	{
		// Pop the return value to a safe place
		if (cur_class->cur_function->GetReturnType().id != VARIABLE_TYPEID_VOID)
			g_Object->WriteOp(OPCODE_POP_RETURN);

		// Write the return code
		cur_class->cur_function->WriteReturn(g_Object);
	}

	outfunc;
}


void CodeGenerator::ProcessBeginTerm(int flags)
{
	infunc(CodeGenerator::ProcessBeginTerm);

	INC_TOKEN;

	if ((flags & FLAGS_IN_STATE) && CUR_TOKEN.type == TOKEN_COLON)
	{
		if (g_Object)
		{
			if (cur_class->cur_state->GetCodeStart() != -1)
				throw CompileError("(Line %d) Multiple 'begin' declaration", CUR_TOKEN.line);

			// Set the execution position here
			cur_class->cur_state->SetCodeStart(g_Object->GetPosition());
		}
	}
	else
	{
		throw CompileError("(Line %d) Expecting colon to complete 'begin' declaration", CUR_TOKEN.line);
	}

	outfunc;
}


void CodeGenerator::ProcessSetstateTerm(int flags)
{
	infunc(CodeGenerator::ProcessSetstateTerm);

	INC_TOKEN;

	// Check for the name
	if (CUR_TOKEN.type != TOKEN_NAME)
		throw CompileError("(Line %d) Expecting the name of a state", CUR_TOKEN.line);

	// Grab the state name
	IsolateTokenString(CUR_TOKEN);

	if (g_Object)
	{
		State	*state_ptr;

		// Try and get the state
		if ((state_ptr = cur_class->GetState(token_string)) == NULL)
			throw CompileError("(Line %d) Undefined state", CUR_TOKEN.line);

		// Write the opcode to set the state
		g_Object->WriteOp(OPCODE_SETSTATE, state_ptr->GetIndex());
	}

	// State name
	INC_TOKEN;

	outfunc;
}


void CodeGenerator::ProcessKeywordTerm(int flags)
{
	infunc(CodeGenerator::ProcessKeyword);

	/* --

	All keywords found at the beginning of a line usually enforce changes to the current
	class. Those which do not are left for the parse tree to deal with.

	-- */

	switch (CUR_TOKEN.type)
	{
		case (TOKEN_IF):
			ProcessIfTerm(flags);
			break;

		case (TOKEN_FOR):
			ProcessForTerm(flags);
			break;

		case (TOKEN_BREAK):
			ProcessBreakTerm(flags);
			break;

		case (TOKEN_CONTINUE):
			ProcessContinueTerm(flags);
			break;

		case (TOKEN_WHILE):
			ProcessWhileTerm(flags);
			break;

		case (TOKEN_DECLARE_CLASS):
			ProcessClassTerm(flags);
			break;

		case (TOKEN_DECLARE_STATE):
			ProcessStateTerm(flags);
			break;

		case (TOKEN_DECLARE_FUNCTION):
			ProcessFunctionTerm(flags);
			break;

		case (TOKEN_DECLARE_CHAR):
			ProcessVarTerm(flags);
			break;

		case (TOKEN_DECLARE_SHORT):
			ProcessVarTerm(flags);
			break;

		case (TOKEN_DECLARE_INT):
			ProcessVarTerm(flags);
			break;

		case (TOKEN_DECLARE_FLOAT):
			ProcessVarTerm(flags);
			break;

		case (TOKEN_DECLARE_STRING):
			ProcessVarTerm(flags);
			break;

		case (TOKEN_RETURN):
			ProcessReturnTerm(flags);
			break;

		case (TOKEN_DECLARE_UNSIGNED):
			ProcessVarTerm(flags);
			break;

		case (TOKEN_DECLARE_SIGNED):
			ProcessVarTerm(flags);
			break;

		case (TOKEN_BEGIN):
			ProcessBeginTerm(flags);
			break;

		case (TOKEN_SETSTATE):
			ProcessSetstateTerm(flags);
			break;

		case (TOKEN_EXTENDS):
			throw CompileError("(Line %d) Illegal use of 'extends' keyword", CUR_TOKEN.line);
			break;

		case (TOKEN_ABSTRACT):
			throw CompileError("(Line %d) Illegal use of 'abstract' keyword", CUR_TOKEN.line);
			break;

		default:
			INC_TOKEN;
			break;
	}

	outfunc;
}


void CodeGenerator::ProcessTerm(int flags)
{
	infunc(CodeGenerator::ProcessTerm);

	/* ----

	Some keywords do not need an explicit parse tree built for the entire statement. Only
	build parse trees for names/numbers and operators; any errors should be correctly
	picked up by the parse tree builder.

	Two names in a row mean a custom variable type declaration.

	---- */

	switch (CUR_TOKEN.Group())
	{
		case (TOKEN_GROUP_NAME):
			if (NEXT_TOKEN.type == TOKEN_NAME)
				ProcessVarTerm(flags);
			else
				ProcessParseTree(flags);
			break;

		case (TOKEN_GROUP_NUMBER):
		case (TOKEN_GROUP_OPERATOR):
			ProcessParseTree(flags);
			break;

		case (TOKEN_GROUP_KEYWORD):
			ProcessKeywordTerm(flags);
			break;
	}

	outfunc;
}


void CodeGenerator::ProcessStatement(int flags)
{
	infunc(CodeGenerator::ProcessStatement);

	// Neutralise the effect of a return in a function
	if (flags & FLAGS_IN_FUNCTION)
		cur_class->cur_function->had_return = 0;

	// Process the first term. This term should be enough to launch a statement of it's
	// own which ends with a semi-colon. Step over this token afterwards.
	ProcessTerm(flags & ~FLAGS_IGNORE_END_TOKEN);

	// The end token is only ignored in one-line if-else statements/for-loops, etc - let it fall through to be
	// picked up by the parent keyword
	if (!(flags & FLAGS_IGNORE_END_TOKEN))
		INC_TOKEN;

	outfunc;
}


void CodeGenerator::Begin(void)
{
	infunc(CodeGenerator::Begin);

	// Begin processing the coooode!

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


void CodeGenerator::GetTypeInfo(VarType &type, int standard_types)
{
	infunc(CodeGenerator::GetTypeInfo);

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
			IsolateTokenString(CUR_TOKEN);
			strcpy(type.name, token_string);
			INC_TOKEN;
			break;
	}

	// Not an array by default
	type.array = 0;
	type.elements = 0;

	outfunc;
}


void CodeGenerator::GetArrayInfo(VarType &type)
{
	infunc(CodeGenerator::GetArrayInfo);

	ParseTree		*tree;
	ParseTreeNode	*top;

	// Should be at the array opening here
	INC_TOKEN;

	// Allocate the parse tree for the expression
	if ((tree = new ParseTree) == NULL)
		throw CError("Couldn't allocate parse tree");

	// Build the tree, ending at the array close
	tree->Build(tokeniser, TOKEN_ARRAY_CLOSE, TOKEN_NULL);
	INC_TOKEN;

	// Reduce the tree to a single constant
	tree->Optimise();

	// Grab the top tree node
	if ((top = tree->GetTop()) == NULL)
		throw CompileError("(Line %d) No dimension for array", CUR_TOKEN.line);

	// Is it a constant?
	if (top->GetNodeType() != PTNODE_TYPE_CONSTANT || top->children[0] || top->children[1])
		throw CompileError("(Line %d) Array size is not a constant", CUR_TOKEN.line);

	// Check for floating point dimensions
	if (top->out_type.id == VARIABLE_TYPEID_FLOAT)
		throw CompileError("(Line %d) Cannot specify array dimension using floats", CUR_TOKEN.line);

	// Retrieve the correct value
	switch (top->out_type.id)
	{
		case (VARIABLE_TYPEID_CHAR):
			type.elements = (int)((Constant *)top)->value.c;
			break;

		case (VARIABLE_TYPEID_SHORT):
			type.elements = (int)((Constant *)top)->value.s;
			break;

		case (VARIABLE_TYPEID_INT):
			type.elements = (int)((Constant *)top)->value.i;
			break;

		case (VARIABLE_TYPEID_UCHAR):
			type.elements = (int)((Constant *)top)->value.uc;
			break;

		case (VARIABLE_TYPEID_USHORT):
			type.elements = (int)((Constant *)top)->value.us;
			break;

		case (VARIABLE_TYPEID_UINT):
			type.elements = (int)((Constant *)top)->value.ui;
			break;
	}

	// Set it as an array
	type.array = 1;

	// The tree is no longer needed
	delete tree;

	outfunc;
}


void CodeGenerator::ReadConstant(dynamic *num, VarType &type, _TokenType terminator, _TokenType extra)
{
	infunc(CodeGenerator::ReadConstant);

	ParseTree		*tree;
	ParseTreeNode	*top;

	// Allocate the parse tree
	if ((tree = new ParseTree) == NULL)
		throw CError("Couldn't allocate parse tree");

	// Build the tree
	tree->Build(tokeniser, terminator, extra);

	// Reduce to a single constant
	tree->Optimise();

	// Grab the top of the tree
	if ((top = tree->GetTop()) == NULL)
		throw CompileError("(Line %d) No constant defined", CUR_TOKEN.line);

	// Get to see if the reduction produces a constant
	if (top->GetNodeType() != PTNODE_TYPE_CONSTANT || top->children[0] || top->children[1])
		throw CompileError("(Line %d) Definition not a constant", CUR_TOKEN.line);

	// Check for a floating point definition in the place of an integer
	if (type.id != VARIABLE_TYPEID_FLOAT && top->out_type.id == VARIABLE_TYPEID_FLOAT)
		throw CompileError("(Line %d) Type mismatch", CUR_TOKEN.line);

	// Scan for the constant with the intended type
	switch (type.id)
	{
		// char
		case (VARIABLE_TYPEID_CHAR):
			num->i = ((Constant *)top)->value.i;
			if (num->i > 127 || num->i < -128)
				throw CompileError("(Line %d) Constant out of range for variable type 'char'", CUR_TOKEN.line);
			break;

		// short
		case (VARIABLE_TYPEID_SHORT):
			num->i = ((Constant *)top)->value.i;
			if (num->i > 32767 || num->i < -32768)
				throw CompileError("(Line %d) Constant out of range for variable type 'short'", CUR_TOKEN.line);
			break;

		// int
		case (VARIABLE_TYPEID_INT):
			num->i = ((Constant *)top)->value.i;
			break;

		// unsigned char
		case (VARIABLE_TYPEID_UCHAR):
			num->i = ((Constant *)top)->value.i;
			if (num->i > 255 || num->i < 0)
				throw CompileError("(Line %d) Constant out of range for variable type 'unsigned char'", CUR_TOKEN.line);
			break;

		// unsigned short
		case (VARIABLE_TYPEID_USHORT):
			num->i = ((Constant *)top)->value.i;
			if (num->i > 65535 || num->i < 0)
				throw CompileError("(Line %d) Constant out of range for variable type 'unsigned short'", CUR_TOKEN.line);
			break;

		// unsigned int
		case (VARIABLE_TYPEID_UINT):
			num->ui = ((Constant *)top)->value.ui;
			break;

		// float
		case (VARIABLE_TYPEID_FLOAT):
			num->f = ((Constant *)top)->value.f;
			break;

		case (VARIABLE_TYPEID_STRING):
			num->i = ((StringLiteral *)top)->string->GetAddress();
			break;
	}

	outfunc;
}
