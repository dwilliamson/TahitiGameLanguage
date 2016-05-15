
#ifndef	_INCLUDED_CODEGENERATOR_H
#define	_INCLUDED_CODEGENERATOR_H


class CodeGenerator
{
public:
	// Constructor/Destructor
	CodeGenerator(Tokeniser *_tokeniser, char *_data);
	~CodeGenerator(void);

	void			Begin(void);

private:
	enum BlockType
	{
		BT_STATEMENT,
		BT_SCOPED
	};

	void			ProcessStatement(int flags);
	void			ProcessTerm(int flags);
	void			ProcessKeywordTerm(int flags);
	void			ProcessClassTerm(int flags);
	void			ProcessStateTerm(int flags);
	void			ProcessFunctionTerm(int flags);
	BlockType		ProcessBlock(int flags);
	void			ProcessReturnTerm(int flags);
	void			ProcessIfTerm(int flags);
	void			ProcessForTerm(int flags);
	void			ProcessBreakTerm(int flags);
	void			ProcessContinueTerm(int flags);
	void			ProcessWhileTerm(int flags);
	void			ProcessVarTerm(int flags);
	void			ProcessBeginTerm(int flags);
	void			ProcessSetstateTerm(int flags);

	void			ProcessParseTree(int flags);

	void			ProcessClassModifiers(Class *class_ptr);

	// Read a constant from the input file (token_string has to be setup)
	void			ReadConstant(dynamic *num, VarType &type, _TokenType terminator, _TokenType extra);

	// Read the text that constructs a token
	void			IsolateTokenString(Token &token);
	char			token_string[MAX_LABEL_SIZE];

	// Get type information about the current token
	void			GetTypeInfo(VarType &type, int standard_types);

	// Get array information about the current token
	void			GetArrayInfo(VarType &type);

	Class							*cur_class;

	// Where to grab the tokens from
	Tokeniser	*tokeniser;

	// Pointer to the actual text data
	char		*data;
};


#endif	/* _INCLUDED_CODEGENERATOR_H */