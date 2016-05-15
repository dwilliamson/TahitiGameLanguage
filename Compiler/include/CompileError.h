
#ifndef	_INCLUDED_COMPILEERROR_H
#define	_INCLUDED_COMPILEERROR_H


class CompileError
{
public:
	// Used in the throw statement
	CompileError(const char *format, ...);

	// Used to retrieve the error message in the catch statement
	char		*GetMessage(void);

private:
	char		message[512];
};


#endif	/* _INCLUDED_COMPILEERROR_H */
