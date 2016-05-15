
#ifndef	_INCLUDED_TOKENISER_H
#define	_INCLUDED_TOKENISER_H


// Types of tokens that will follow after reading the first character
#define	CHARTYPE_IGNORE			0
#define	CHARTYPE_DIGIT			1
#define	CHARTYPE_ALPHA			2
#define CHARTYPE_OPERATOR		3
#define CHARTYPE_MISC			4


// Utility macros
#define CUR_CHAR_TYPE	(FirstCharTable[CUR_CHAR])		// Current character type
#define INC_POSITION	(position++)					// Move onto next character
#define GET_CHAR		(data[position++])				// Get the current character and move
#define BYTES_LEFT		(position < filesize)			// Any bytes left in the file
#define	CUR_CHAR		(data[position])				// Current character
#define NEXT_CHAR		(data[position + 1])			// Next character


#define TOKEN_PACKET_SIZE		1000


#define TOKENISER_VMC		1
#define	TOKENISER_IDL		2


class Keyword : public CHashCell
{
public:
	// Token type this is associated with
	_TokenType	type;
};


class Tokeniser
{
public:
	// Constructor/Destructor
	Tokeniser(char *_data, int _size, int type);
	~Tokeniser(void);

	// The current token packet
	TArray<Token>	token_list;
	int				token_pos;

	// Move onto the next token
	void			IncToken(void);

	// Isolate a given token string in the current file
	void			IsolateString(Token &token, char *store);

	// Data to tokenise
	char			*data;

private:
	// Manage tokenisation
	int				AddToken(void);
	int				GenerateTokenPacket(void);

	// Specific token addition
	int				AddAlphaToken(void);
	int				AddNumberToken(void);
	int				AddOperatorToken(void);
	int				AddMiscToken(void);

	int				RegisterKeyword(const char *keyword, _TokenType type);

	// Size of the current token packet
	int				packet_size;

	// Comment flags
	int				in_cpp_comment;

	// Character position within the file
	int				position;

	// Current line within the file
	int				current_line;

	// Tokenising initialisation
	char			FirstCharTable[256];

	// Size of file to tokenise
	int				filesize;

	THash<>			keyword_table;
};


#endif	/* _INCLUDED_TOKENISER_H */