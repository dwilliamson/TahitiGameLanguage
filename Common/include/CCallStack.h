
#ifndef	_INCLUDED_CCALLSTACK_H
#define	_INCLUDED_CCALLSTACK_H

#ifdef	CALLSTACK_ENABLED

	#define infunc(x)	static const char *__FUNC_NAME__ = #x; try {
	#define outfunc		} catch (...) { CALLSTACK_CLASS.AddFunction(__FUNC_NAME__); throw; }

#else

	#define	infunc(x)
	#define	outfunc

#endif


class CCallStack
{
public:
	// Constructor
	CCallStack(void);

	// Add a function to the call stack
	int				AddFunction(const char *name);

	// Enumerate all the function names on the stack
	const char		*EnumerateFunctions(void);

private:
	// Array of function name strings
	TArray<const char *>	string_list;

	// Current Enumeration position
	int						enum_pos;
};


#endif	/* _INCLUDED_CCALLSTACK_H */