
#include "..\include\main.h"


Expression::Expression(Tokeniser *tokeniser, _TokenType terminator, _TokenType extra)
{
	infunc(Expression::Expression);

	// Assign defaults
	parent = NULL;
	children[0] = NULL;
	children[1] = NULL;

	// Allocate the parse tree
	if ((tree = new ParseTree) == NULL)
		throw CError("Couldn't allocate parse tree for expression");

	// Build the tree from the current position
	tree->Build(tokeniser, terminator, extra);

	outfunc;
}


Expression::~Expression(void)
{
	infunc(Expression::~Expression);

	// Only the tree needs to go
	delete tree;

	outfunc;
}


void Expression::GenerateCode(int flags)
{
	infunc(Expression::GenerateCode);

	// Generate this tree
	tree->GenerateCode(flags);

	outfunc;
}


void Expression::Optimise(ParseTreeNode **holder)
{
	infunc(Expression::Optimise);

	// Optimise this tree
	tree->Optimise();

	outfunc;
}


void Expression::ResolveType(int flags)
{
	infunc(Expression::ResolveType);
	
	ParseTreeNode	*node;

	node = tree->GetTop();

	// Resolve types for this tree
	if (node)
		node->ResolveType(flags);

	// Get the type as a result of an expression
	out_type = tree->GetTop()->out_type;
	in_type.Make(VARIABLE_TYPEID_VOID);
	token = tree->GetTop()->token;

	outfunc;
}


int Expression::GetNodeType(void)
{
	infunc(Expression::GetNodeType);

	return (PTNODE_TYPE_EXPRESSION);

	outfunc;
}


void Expression::DebugPrint(int indent, char *data)
{
	infunc(Expression::DebugPrint);

	// Write the tree
	tree->Debug();

	outfunc;
}