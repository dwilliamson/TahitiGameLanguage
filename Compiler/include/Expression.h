
#ifndef	_INCLUDED_EXPRESSION_H
#define	_INCLUDED_EXPRESSION_H


class Expression : public ParseTreeNode
{
public:
	// Constructor/Destructor
	Expression(Tokeniser *tokeniser, _TokenType terminator, _TokenType extra);
	~Expression(void);

	// Generate code for the expression
	void		GenerateCode(int flags);

	// Optimise this node if possible
	void		Optimise(ParseTreeNode **holder);

	// Figure out the type returned by this node
	void		ResolveType(int flags);

	// Retrieve the type of node this is
	int			GetNodeType(void);

	// Print the information about this node
	void		DebugPrint(int indent, char *data);

	// Parse tree associated with this expression
	ParseTree	*tree;
};


#endif	/* INCLUDED_EXPRESSION_H */