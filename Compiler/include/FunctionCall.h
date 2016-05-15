
#ifndef	_INCLUDED_FUNCTIONCALL_H
#define	_INCLUDED_FUNCTIONCALL_H


class FunctionCall : public ParseTreeNode
{
public:
	// Constructor/Destructor
	FunctionCall(Tokeniser *tokeniser);
	~FunctionCall(void);

	// Generate code for the function call
	void		GenerateCode(int flags);

	// Optimise this node if possible
	void		Optimise(ParseTreeNode **holder);

	// Figure out the type returned by this node
	void		ResolveType(int flags);

	// Retrieve the type of node this is
	int			GetNodeType(void);

	// Print the information about this node
	void		DebugPrint(int indent, char *data);

private:
	// List of expressions passed as parameters
	TArray<Expression *, 5>	param_list;

	// Pointer to the function being called
	Function	*function;
};


#endif	/* _INCLUDED_FUNCTIONCALL_H */