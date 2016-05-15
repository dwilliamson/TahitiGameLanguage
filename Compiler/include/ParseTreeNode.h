
#ifndef	_INCLUDED_PARSETREENODE_H
#define	_INCLUDED_PARSETREENODE_H


#define	PTNODE_TYPE_NORMAL				0
#define PTNODE_TYPE_EXPRESSION			1
#define PTNODE_TYPE_FUNCTIONCALL		2
#define PTNODE_TYPE_CONSTANT			3
#define PTNODE_TYPE_STRINGLITERAL		4


class ParseTreeNode
{
public:
	// Destructor
	virtual			~ParseTreeNode(void);

	// Optimise this node if possible
	virtual void	Optimise(ParseTreeNode **holder);

	// Figure out the type returned by this node
	virtual void	ResolveType(int flags);

	// Generate the code for this node (and children)
	virtual void	GenerateCode(int flags);

	// Retrieve the type of node this is
	virtual int		GetNodeType(void);

	// Make the links for a left child
	void			AssignLeftChild(ParseTreeNode *child);

	// Make the links for a right child
	void			AssignRightChild(ParseTreeNode *child);

	// Write the conversion opcode necessary for converting from one type to the other
	void			WriteConverter(int from, int to);

	// Print the information about this node
	virtual void	DebugPrint(int indent, char *data);

	// Node shared parent
	ParseTreeNode	*parent;

	// List of children
	ParseTreeNode	*children[2];

	// Type of variable produced from this node
//	VarType			type;

	VarType			in_type, out_type;

	// Token at this node
	Token			token;

	// Pointer to tokeniser for text retrieval
	Tokeniser		*tokeniser;
};


#endif	/* _INCLUDED_PARSETREENODE_H */