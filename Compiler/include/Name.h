
#ifndef	_INCLUDED_NAME_H
#define	_INCLUDED_NAME_H


class Name : public ParseTreeNode
{
public:
	// Constructor/Destructor
	Name(Tokeniser *tokeniser);
	~Name(void);

	// Pointer to the variable this name references
	Variable	*variable;

	// Generate some code for this variable reference
	void		GenerateCode(int flags);

	// Figure out the type returned by this node
	void		ResolveType(int flags);

private:
	void		PushAddress(void);
	void		PushValue(int array);
};


#endif	/* _INCLUDED_NAME_H */