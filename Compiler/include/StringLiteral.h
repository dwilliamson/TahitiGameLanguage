
#ifndef	_INCLUDED_STRINGLITERAL_H
#define _INCLUDED_STRINGLITERAL_H


class StringLiteral : public ParseTreeNode
{
public:
	// Constructor
	StringLiteral(Tokeniser *tokeniser);

	// Generate the code for this node
	void		GenerateCode(int flags);

	// Figure out the type returned by this node
	void		ResolveType(int flags);

	// Retrieve the type of node this is
	int			GetNodeType(void);

	// Variable the points to the address of the string literal
	Variable	*string;
};


#endif	/* _INCLUDED_STRINGLITERAL_H */