
#ifndef	_INCLUDED_IDLPARSER_H
#define	_INCLUDED_IDLPARSER_H


class IDLParser
{
public:
	// Constructor
	IDLParser(Tokeniser *_tokeniser, char *_data);

	// Start processing the IDL file
	void	Begin(void);

private:
	// Tokeniser for current data file
	Tokeniser	*tokeniser;

	// Currently active interface
	Class	*cur_interface;

	// Private stuff
	void	ProcessTerm(int flags);
	void	ProcessKeywordTerm(int flags);
	void	ProcessInterfaceTerm(int flags);
	void	ProcessBlock(int flags);
	void	ProcessStatement(int flags);
	void	ProcessMethodTerm(int flags);

	// Get type information at the current token position
	void	GetTypeInfo(VarType &type, int standard_types);

	// Get the type array information at the current token position
	void	GetArrayInfo(VarType &type);

	// Current data file in memory
	char		*data;
};


#endif	/* _INCLUDED_IDLPARSER_H */