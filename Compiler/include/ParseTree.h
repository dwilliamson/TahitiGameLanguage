
#ifndef	_INCLUDED_PARSETREE_H
#define	_INCLUDED_PARSETREE_H


class ParseTree
{
public:
	// Destructor
	~ParseTree(void);

	// Build a parse tree from the current position in the tokeniser
	void			Build(Tokeniser *_tokeniser, _TokenType terminator, _TokenType extra);

	// Reduce any redundant calculations
	void			Optimise(void);

	void			CompleteTypes(int flags);

	// Write the byte-code associated with this parse tree
	void			GenerateCode(int flags);

	// Get the top of the parse tree
	ParseTreeNode	*GetTop(void);

	// Debug trace from the top of the tree
	void			Debug(void);

private:
	// Allocate an initialise a new node
	ParseTreeNode	*NewNode(void);

	// Locate the top of the tree given any node
	void			LocateTop(ParseTreeNode *from);

	// Miscellaneous private processing functions
	ParseTreeNode	*ProcessTerm(ParseTreeNode *left);
	ParseTreeNode	*ProcessNameTerm(ParseTreeNode *left);
	ParseTreeNode	*ProcessNumberTerm(ParseTreeNode *left);
	ParseTreeNode	*ProcessOperatorTerm(ParseTreeNode *left);
	ParseTreeNode	*ProcessNormalOpTerm(ParseTreeNode *left);
	ParseTreeNode	*ProcessStringTerm(ParseTreeNode *left);

	// Top of the parse tree
	ParseTreeNode	*top;

	// Tokeniser
	Tokeniser		*tokeniser;

	// Where code is written to
	VMOFile			*object;
};


#endif	/* _INCLUDED_PARSETREE_H */