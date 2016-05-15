
#ifndef	_INCLUDED_CONSTANT_H
#define	_INCLUDED_CONSTANT_H


class Constant : public ParseTreeNode
{
public:
	// Constructor
	Constant(char *token_string);

	// Generate the code for this node (and children)
	void		GenerateCode(int flags);

	// Figure out the type returned by this node
	void		ResolveType(int flags);

	// Retrieve the type of node this is
	int			GetNodeType(void);

	// Constant value
	dynamic		value;
};


#endif	/* _INCLUDED_CONSTANT_H */