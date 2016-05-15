
#include "..\include\main.h"


ParseTree::~ParseTree(void)
{
	// Delete the entire tree from the top down
	if (top)
		delete top;
}


ParseTreeNode *ParseTree::NewNode(void)
{
	infunc(ParseTree::NewNode);

	ParseTreeNode	*node;

	// Allocate
	if ((node = new ParseTreeNode) == NULL)
		throw CError("Couldn't allocate parse tree node");

	// Initialise
	node->parent = NULL;
	node->children[0] = NULL;
	node->children[1] = NULL;

	return (node);

	outfunc;
}


void ParseTree::LocateTop(ParseTreeNode *from)
{
	infunc(ParseTree::LocateTop);

	if (from == NULL)
	{
		top = NULL;
		return;
	}

	// Loop until the current node has no parent
	while (from->parent)
		from = from->parent;

	// This is the top of the tree
	top = from;

	outfunc;
}


ParseTreeNode *ParseTree::ProcessNameTerm(ParseTreeNode *left)
{
	infunc(ParseTree::ProcessNameTerm);

	ParseTreeNode	*node;
	Token			token;

	// Grab the name token
	token = CUR_TOKEN;
	INC_TOKEN;

	// Is it a function call?
	if (CUR_TOKEN.type == TOKEN_LEFT_BRACKET)
	{
		// Start a function call
		if ((node = new FunctionCall(tokeniser)) == NULL)
			throw CError("Couldn't allocate function call structure");

		// Assign the name
		node->token = token;
	}
	else
	{
		// Process the new name
		if ((node = new Name(tokeniser)) == NULL)
			throw CError("Couldn't allocate name structure");
	}

	return (node);

	outfunc;
}


ParseTreeNode *ParseTree::ProcessNumberTerm(ParseTreeNode *left)
{
	infunc(ParseTree::ProcessNumberTerm);

	Constant		*node;
	char			token_string[MAX_LABEL_SIZE];
	int				x;

	// Read the token string and terminate it
	for (x = 0; x < CUR_TOKEN.length; x++)
		token_string[x] = tokeniser->data[CUR_TOKEN.position + x];
	token_string[x] = 0;

	// Allocate and process
	if ((node = new Constant(token_string)) == NULL)
		throw CError("Failed to allocated constant structure");

	// Grab the name token
	node->token = CUR_TOKEN;
	INC_TOKEN;

	return (node);

	outfunc;
}


ParseTreeNode *ParseTree::ProcessNormalOpTerm(ParseTreeNode *left)
{
	infunc(ParseTree::ProcessNormalOpTerm);

	ParseTreeNode	*node;

	/* ----

	If the current operator is an opening bracket then no new node should be created (it
	treats it as a unary node). Instead, pass over the opening bracket and continue
	processing the sub-expression (like the ParseTree::Build function) until the closing
	bracket is encountered.

	---- */

	if (CUR_TOKEN.type == TOKEN_LEFT_BRACKET)
	{
		ParseTreeNode	*left = NULL;

		// Open bracket
		INC_TOKEN;

		while (CUR_TOKEN.type != TOKEN_RIGHT_BRACKET)
			left = ProcessTerm(left);

		// Close bracket
		INC_TOKEN;

		return (left);
	}

	// Get a new node
	node = NewNode();

	// Grab the operator token
	node->token = CUR_TOKEN;
	INC_TOKEN;

	/* ---------

	The left child has been passed up to here, get the right child by processing the next
	term. This function will always return a parent. If the next term happens to be an
	operator then it must be a unary operator. This case is taken care of flawlessly since
	a left child of NULL is passed to the next term, resulting in a single child parent.

	The two special cases (a++, a--) are needed since they are not unary operators, they
	are operators with effect after the original value has been used in the calculation.
	This sequence of code will generate an operator parent with a single left child. If the
	operation was unary it would be a right child parent, allowing the code to distinguish
	between post and pre-effect operators.

	--------- */

	if (left != NULL &&
	   (node->token.type == TOKEN_PLUS_PLUS || node->token.type == TOKEN_MINUS_MINUS))
	{
		node->AssignLeftChild(left);
		node->AssignRightChild(NULL);

		return (node);
	}

	node->AssignLeftChild(left);
	node->AssignRightChild(ProcessTerm(NULL));

	/* ----

	At the next operator here. Operators will always make parents. If the operator here
	has a higher priority than the one that is this parent then it needs to be processed
	first. Therefore, make it a right child of this parent, passing the just defined
	right child as the operators left child.

	---- */

	while (CUR_TOKEN.Priority() > node->token.Priority())
		node->AssignRightChild(ProcessTerm(node->children[1]));

	return (node);

	outfunc;
}


ParseTreeNode *ParseTree::ProcessOperatorTerm(ParseTreeNode *left)
{
	infunc(ParseTree::ProcessOperatorTerm);

	ParseTreeNode	*node;

	/* -----

	Need to distinguish between normal operators and assignment operators since assignment
	operators need a ParseTreeNode-inherited Expression node at the right child, with a
	normal node residing on the left side.

	If the operator turns out to be an array indexer then it needs to inherit the current
	left child and have the expression determining the index at the right child.

	----- */

	if (CUR_TOKEN.Is(TOKEN_TYPE_ASSIGNMENT))
	{
		Expression	*expr;

		// Get a new node
		node = NewNode();

		// Grab the operator token
		node->token = CUR_TOKEN;
		INC_TOKEN;

		// Allocate and process the expression
		if ((expr = new Expression(tokeniser, TOKEN_END_OF_LINE, TOKEN_NULL)) == NULL)
			throw CError("Couldn't allocate expression class");

		node->AssignLeftChild(left);
		node->AssignRightChild(expr);
	}

	else if (CUR_TOKEN.type == TOKEN_ARRAY_OPEN)
	{
		Expression	*expr;

		// Get a new node
		node = NewNode();

		// Grab the operator token
		node->token = CUR_TOKEN;
		INC_TOKEN;

		// Allocate and process the expression
		if ((expr = new Expression(tokeniser, TOKEN_ARRAY_CLOSE, TOKEN_NULL)) == NULL)
			throw CError("Couldn't allocate expression class");

		// Skip passed the close
		INC_TOKEN;

		node->AssignLeftChild(left);
		node->AssignRightChild(expr);
	}

	else
	{
		node = ProcessNormalOpTerm(left);
	}

	return (node);

	outfunc;
}


ParseTreeNode *ParseTree::ProcessStringTerm(ParseTreeNode *left)
{
	infunc(ParseTree::ProcessStringTerm);

	StringLiteral	*node;

	// Allocate and process the string literal
	if ((node = new StringLiteral(tokeniser)) == NULL)
		throw CError("Couldn't allocate string literal");

	return (node);

	outfunc;
}


ParseTreeNode *ParseTree::ProcessTerm(ParseTreeNode *left)
{
	infunc(ParseTree::ProcessTerm);

	ParseTreeNode	*node;

	switch (CUR_TOKEN.Group())
	{
		case (TOKEN_GROUP_NUMBER):
			// Branch off
			node = ProcessNumberTerm(left);
			break;

		case (TOKEN_GROUP_NAME):
			// Branch off
			node = ProcessNameTerm(left);
			break;

		case (TOKEN_GROUP_OPERATOR):
			// Branch off
			node = ProcessOperatorTerm(left);
			break;

		case (TOKEN_GROUP_NONE):
			/* ---

			The end of file token should never be a part of the code. The outer compiling
			loop makes sure it never processes this far. This means that if the NULL token
			is encountered, the current expression has not been finished.

			--- */

			if (CUR_TOKEN.type == TOKEN_NULL)
				throw CompileError("(Line %d) Unexpected end of file", CUR_TOKEN.line);
			break;

		case (TOKEN_GROUP_MISC):
			/* -----

			The end of line character should always start a new term of its own. The
			expression function will make sure however that it never gets this far and
			terminates the expression. If an end of line character is picked up here then
			this means that the EOL character is within a term, something which is
			obviously illegal. The same too, applies to the closing block bracket.

			----- */

			if (CUR_TOKEN.type == TOKEN_END_OF_LINE)
				throw CompileError("(Line %d) Unexpected end of line", CUR_TOKEN.line);

			if (CUR_TOKEN.type == TOKEN_BLOCK_CLOSE)
				throw CompileError("(Line %d) Unexpected closing '}'", CUR_TOKEN.line);

			if (CUR_TOKEN.type == TOKEN_STRING_LITERAL)
				node = ProcessStringTerm(left);
			break;
	}

	return (node);

	outfunc;
}


void ParseTree::Build(Tokeniser *_tokeniser, _TokenType terminator, _TokenType extra)
{
	infunc(ParseTree::Build);

	ParseTreeNode	*left = NULL;

	// Point to the tokeniser
	tokeniser = _tokeniser;

	while (1)
	{
		if (CUR_TOKEN.type == terminator)
			break;

		// Check for the optional terminator
		if (extra != TOKEN_NULL)
		{
			if (CUR_TOKEN.type == extra)
				break;
		}

		// Process each term, assigning it to be the left child upon each completion
		left = ProcessTerm(left);
	}

	// Point to the top of the tree
	LocateTop(left);

	outfunc;
}


void ParseTree::Optimise(void)
{
	infunc(ParseTree::Optimise);

	if (top == NULL)
		return;

	// Optimise from the top level node
	top->Optimise(&top);

	outfunc;
}


void ParseTree::CompleteTypes(int flags)
{
	infunc(ParseTree::CompleteTypes);

	if (top == NULL)
		return;

	// Resolve types from the top of the tree
	top->ResolveType(flags);

	outfunc;
}


void ParseTree::GenerateCode(int flags)
{
	infunc(ParseTree::GenerateCode);

	if (top == NULL)
		return;

	// Generate code from the top level node
	top->GenerateCode(flags);

	outfunc;
}


ParseTreeNode *ParseTree::GetTop(void)
{
	return (top);
}


void ParseTree::Debug(void)
{
	infunc(ParseTree::Debug);

	// Debug from the top level node
	top->DebugPrint(0, tokeniser->data);

	outfunc;
}