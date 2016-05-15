
#ifndef	_INCLUDED_VARTYPE_H
#define	_INCLUDED_VARTYPE_H


class VarType
{
public:
	// Overloaded assignment
	void		operator = (VarType &other);

	// Overloaded comparison
	int			operator == (VarType &other);

	// Overloaded not comparison
	int			operator != (VarType &other);

	// Construct a default variable type
	void		Make(int what);

	// Type ID
	int			id;

	// Name representing type if it's a custom one
	char		name[MAX_LABEL_SIZE];

	// Is this an array?
	int			array;

	// Number of elements if it's an array
	int			elements;
};


#endif	/* _INCLUDED_VARTYPE_H */