
#ifndef	_INCLUDED_METHOD_H
#define	_INCLUDED_METHOD_H


class Method
{
public:
	// Constructor
	Method(void) { m_Comment = NULL; }

	// Name of the method
	char	m_Name[256];

	// String describing what it returns
	char	m_Returns[256];

	// String describing what parameters it takes
	char	m_Parameters[256];

	// Comment associated with the token
	char	*m_Comment;
};


#endif	/* _INCLUDED_METHOD_H */