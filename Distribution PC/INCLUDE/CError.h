
#ifndef	_INCLUDED_CERROR_H
#define	_INCLUDED_CERROR_H


class CError
{
public:
	// Constructor for error message with variable arguments
	CError(const char *text, ...);

	char	*GetMessage(void)	{ return (message);	}

private:
	// Error message
	char	message[512];
};


#endif	/* _INCLUDED_CERROR_H */