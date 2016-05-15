
#include "..\include\main.h"


Tokeniser::Tokeniser(char *_data, int _size, int type)
{
	Token	null = { TOKEN_NULL, 0, 0 };

	infunc(Tokeniser::Tokeniser);

	// Set some stuff
	position = 0;
	in_cpp_comment = 0;
	data = _data;
	filesize = _size;
	current_line = 1;

	// Set all characters to ignore initially
	memset(FirstCharTable, CHARTYPE_IGNORE, 256);

	// Set the alphabet characters
	memset(&FirstCharTable['a'], CHARTYPE_ALPHA, 26);
	memset(&FirstCharTable['A'], CHARTYPE_ALPHA, 26);
	FirstCharTable['_'] = CHARTYPE_ALPHA;

	// Set the number characters
	memset(&FirstCharTable['0'], CHARTYPE_DIGIT, 10);

	// Set the operator characters
	FirstCharTable['+'] = CHARTYPE_OPERATOR;
	FirstCharTable['-'] = CHARTYPE_OPERATOR;
	FirstCharTable['*'] = CHARTYPE_OPERATOR;
	FirstCharTable['/'] = CHARTYPE_OPERATOR;
	FirstCharTable['&'] = CHARTYPE_OPERATOR;
	FirstCharTable['^'] = CHARTYPE_OPERATOR;
	FirstCharTable['|'] = CHARTYPE_OPERATOR;
	FirstCharTable['='] = CHARTYPE_OPERATOR;
	FirstCharTable[','] = CHARTYPE_OPERATOR;
	FirstCharTable['('] = CHARTYPE_OPERATOR;
	FirstCharTable[')'] = CHARTYPE_OPERATOR;
	FirstCharTable['%'] = CHARTYPE_OPERATOR;
	FirstCharTable['!'] = CHARTYPE_OPERATOR;
	FirstCharTable['<'] = CHARTYPE_OPERATOR;
	FirstCharTable['>'] = CHARTYPE_OPERATOR;
	FirstCharTable['~'] = CHARTYPE_OPERATOR;
	FirstCharTable['['] = CHARTYPE_OPERATOR;
	FirstCharTable[']'] = CHARTYPE_OPERATOR;

	// Set other miscellaneous characters
	FirstCharTable[';'] = CHARTYPE_MISC;
	FirstCharTable['{'] = CHARTYPE_MISC;
	FirstCharTable['}'] = CHARTYPE_MISC;
	FirstCharTable[':'] = CHARTYPE_MISC;
	FirstCharTable['\"'] = CHARTYPE_MISC;
	FirstCharTable['\''] = CHARTYPE_MISC;

	// Register all the keywords
	RegisterKeyword("float", TOKEN_DECLARE_FLOAT);
	RegisterKeyword("int", TOKEN_DECLARE_INT);
	RegisterKeyword("short", TOKEN_DECLARE_SHORT);
	RegisterKeyword("char", TOKEN_DECLARE_CHAR);
	RegisterKeyword("signed", TOKEN_DECLARE_SIGNED);
	RegisterKeyword("unsigned", TOKEN_DECLARE_UNSIGNED);
	RegisterKeyword("void", TOKEN_VOID);
	RegisterKeyword("string", TOKEN_DECLARE_STRING);

	if (type == TOKENISER_VMC)
	{
		RegisterKeyword("if", TOKEN_IF);
		RegisterKeyword("else", TOKEN_ELSE);
		RegisterKeyword("for", TOKEN_FOR);
		RegisterKeyword("continue", TOKEN_CONTINUE);
		RegisterKeyword("break", TOKEN_BREAK);
		RegisterKeyword("while", TOKEN_WHILE);
		RegisterKeyword("switch", TOKEN_SWITCH);
		RegisterKeyword("case", TOKEN_CASE);
		RegisterKeyword("default", TOKEN_DEFAULT);
		RegisterKeyword("do", TOKEN_DO);
		RegisterKeyword("class", TOKEN_DECLARE_CLASS);
		RegisterKeyword("public", TOKEN_PUBLIC);
		RegisterKeyword("protected", TOKEN_PROTECTED);
		RegisterKeyword("private", TOKEN_PRIVATE);
		RegisterKeyword("state", TOKEN_DECLARE_STATE);
		RegisterKeyword("function", TOKEN_DECLARE_FUNCTION);
		RegisterKeyword("final", TOKEN_FINAL);
		RegisterKeyword("return", TOKEN_RETURN);
		RegisterKeyword("extends", TOKEN_EXTENDS);
		RegisterKeyword("begin", TOKEN_BEGIN);
		RegisterKeyword("abstract", TOKEN_ABSTRACT);
		RegisterKeyword("setstate", TOKEN_SETSTATE);
		// Removed in favour of the IDL
//		RegisterKeyword("import", TOKEN_IMPORT);
	}

	if (type == TOKENISER_IDL)
	{
		RegisterKeyword("interface", TOKEN_INTERFACE);
		RegisterKeyword("extends", TOKEN_EXTENDS);
		RegisterKeyword("method", TOKEN_METHOD);
	}

	// Make sure the token list is ready
	token_list.Reset();
	token_pos = 0;

	// The first token is an illegal token, not to be touched
	token_list.Add(null);

	// Generate the token packet (size plus the initial token)
	packet_size = GenerateTokenPacket() + 1;

	// Has the end of the file been hit?
	if (packet_size != TOKEN_PACKET_SIZE + 2)
	{
		// Shove a NULL token at the end
		token_list.Add(null);
		packet_size++;
	}
	else
	{
		// Cycle the position back to compensate for reading the extra token at the end
		position -= token_list(packet_size - 1).length;
	}

	// Move onto the first legal token
	token_pos = 1;

	outfunc;
}


Tokeniser::~Tokeniser(void)
{
	Keyword	*keyptr;

	infunc(Tokeniser::~Tokeniser);

	// Enumerate all the keywords
	while (keyptr = (Keyword *)keyword_table.Enumerate())
	{
		// Now delete it
		keyword_table.Remove(keyptr);
		delete keyptr;
	}

	outfunc;
}


int Tokeniser::RegisterKeyword(const char *keyword, _TokenType type)
{
	Keyword	*keyptr;

	infunc(Tokeniser::RegisterKeyword);

	// Allocate the memory
	if ((keyptr = new Keyword) == NULL)
		return (0);

	// Copy the data over
	keyptr->type = type;

	// Add to the list
	keyword_table.Add(keyword, keyptr);

	return (1);

	outfunc;
}


int Tokeniser::AddAlphaToken(void)
{
	Token	to_add;
	char	name[MAX_LABEL_SIZE];
	int		x;
	Keyword	*keyptr;

	infunc(Tokeniser::AddAlphaToken);

	// Build the token
	to_add.position = position;
	to_add.type = TOKEN_NAME;
	to_add.length = 0;
	to_add.line = current_line;

	// Loop until the end of the name
	while (CUR_CHAR_TYPE == CHARTYPE_ALPHA || CUR_CHAR_TYPE == CHARTYPE_DIGIT)
	{
		to_add.length++;
		INC_POSITION;
	}

	// Get the actual text
	for (x = 0; x < to_add.length; x++)
		name[x] = data[to_add.position + x];
	name[x] = 0;

	// If it's a keyword, assign the correct token
	if ((keyptr = (Keyword *)keyword_table.GetEntry(name)))
		to_add.type = keyptr->type;

	// Add the token to the list
	token_list.Add(to_add);

	return (1);

	outfunc;
}


int Tokeniser::AddNumberToken(void)
{
	Token	to_add;
	int		had_decimal = 0;

	infunc(Tokeniser::AddNumberToken);

	// Build the token
	to_add.position = position;
	to_add.type = TOKEN_NUMBER;
	to_add.length = 0;
	to_add.line = current_line;

	// Loop until the end of the number
	while (1)
	{
		// Check for a floating point number
		if (CUR_CHAR == '.')
		{
			if (had_decimal)
				throw CompileError("(Line %d) Illegal decimal place", current_line);

			had_decimal = 1;

			to_add.length++;
			INC_POSITION;

			continue;
		}

		if (CUR_CHAR_TYPE != CHARTYPE_DIGIT)
			break;

		to_add.length++;
		INC_POSITION;
	}

	// Add the token to the list
	token_list.Add(to_add);

	return (1);

	outfunc;
}


int Tokeniser::AddOperatorToken(void)
{
	char	cur_char;
	Token	to_add;

	infunc(Tokeniser::AddOperatorToken);

	// Build the token
	to_add.position = position;
	to_add.length = 1;
	to_add.line = current_line;

	// Get the operator character
	cur_char = GET_CHAR;

	#define AddExtendToken(x)	{ to_add.type = x; to_add.length++; INC_POSITION; }

	switch (cur_char)
	{
		case ('+'):
			// Determine what type of token this is
			if (CUR_CHAR == '+')
				AddExtendToken(TOKEN_PLUS_PLUS)
			else if (CUR_CHAR == '=')
				AddExtendToken(TOKEN_PLUS_ASSIGN)
			else
				to_add.type = TOKEN_PLUS;
			break;

		case ('-'):
			// Determine what type of token this is
			if (CUR_CHAR == '-')
				AddExtendToken(TOKEN_MINUS_MINUS)
			else if (CUR_CHAR == '=')
				AddExtendToken(TOKEN_MINUS_ASSIGN)
			else
				to_add.type = TOKEN_MINUS;
			break;

		case ('/'):
			// Determine what type of token this is
			if (CUR_CHAR == '=')
				AddExtendToken(TOKEN_DIVIDE_ASSIGN)
			else if (CUR_CHAR == '/')
			{
				in_cpp_comment = 1;
				INC_POSITION;
				return (1);
			}
			else
				to_add.type = TOKEN_DIVIDE;
			break;

		case ('*'):
			// Determine what type of token this is
			if (CUR_CHAR == '=')
				AddExtendToken(TOKEN_MULTIPLY_ASSIGN)
			else
				to_add.type = TOKEN_MULTIPLY;
			break;

		case ('&'):
			// Determine what type of token this is
			if (CUR_CHAR == '=')
				AddExtendToken(TOKEN_BITWISE_AND_ASSIGN)
			else if (CUR_CHAR == '&')
				AddExtendToken(TOKEN_LOGICAL_AND)
			else
				to_add.type = TOKEN_BITWISE_AND;
			break;

		case ('^'):
			// Determine what type of token this is
			if (CUR_CHAR == '=')
				AddExtendToken(TOKEN_BITWISE_XOR_ASSIGN)
			else
				to_add.type = TOKEN_BITWISE_XOR;
			break;

		case ('|'):
			// Determine what type of token this is
			if (CUR_CHAR == '=')
				AddExtendToken(TOKEN_BITWISE_OR_ASSIGN)
			else if (CUR_CHAR == '|')
				AddExtendToken(TOKEN_LOGICAL_OR)
			else
				to_add.type = TOKEN_BITWISE_OR;
			break;

		case ('='):
			// Determine what type of token this is
			if (CUR_CHAR == '=')
				AddExtendToken(TOKEN_EQUAL)
			else
				to_add.type = TOKEN_ASSIGN;
			break;

		case (','):
			to_add.type = TOKEN_COMMA;
			break;

		case ('('):
			to_add.type = TOKEN_LEFT_BRACKET;
			break;

		case (')'):
			to_add.type = TOKEN_RIGHT_BRACKET;
			break;

		case ('%'):
			// Determine what type of token this is
			if (CUR_CHAR == '=')
				AddExtendToken(TOKEN_MODULUS_ASSIGN)
			else
				to_add.type = TOKEN_MODULUS;
			break;

		case ('!'):
			// Determine what type of token this is
			if (CUR_CHAR == '=')
				AddExtendToken(TOKEN_LOGICAL_NOT_EQUAL)
			else
				to_add.type = TOKEN_LOGICAL_NOT;
			break;

		case ('<'):
			// Determine what type of token this is
			if (CUR_CHAR == '=')
				AddExtendToken(TOKEN_LESS_EQUAL)
			else if (CUR_CHAR == '<')
			{
				if (NEXT_CHAR == '=')
				{
					to_add.type = TOKEN_SHL_ASSIGN;
					to_add.length += 2;
					INC_POSITION;
					INC_POSITION;
				}
				else
					AddExtendToken(TOKEN_SHIFT_LEFT)
			}
			else
				to_add.type = TOKEN_LESS;
			break;

		case ('>'):
			// Determine what type of token this is
			if (CUR_CHAR == '=')
				AddExtendToken(TOKEN_GREATER_EQUAL)
			else if (CUR_CHAR == '>')
			{
				if (NEXT_CHAR == '=')
				{
					to_add.type = TOKEN_SHR_ASSIGN;
					to_add.length += 2;
					INC_POSITION;
					INC_POSITION;
				}
				else
					AddExtendToken(TOKEN_SHIFT_RIGHT)
			}
			else
				to_add.type = TOKEN_GREATER;
			break;

		case ('~'):
			to_add.type = TOKEN_BITWISE_NOT;
			break;

		case ('['):
			to_add.type = TOKEN_ARRAY_OPEN;
			break;

		case (']'):
			to_add.type = TOKEN_ARRAY_CLOSE;
			break;
	}

	#undef	AddExtendToken

	// Add this token to the list
	token_list.Add(to_add);

	return (1);

	outfunc;
}


int Tokeniser::AddMiscToken(void)
{
	Token	to_add;

	infunc(Tokeniser::AddMiscToken);

	// Build the token
	to_add.length = 1;
	to_add.position = position;
	to_add.line = current_line;

	// Decide upon the type of token
	switch (CUR_CHAR)
	{
		case (';'):
			to_add.type = TOKEN_END_OF_LINE;
			break;

		case ('{'):
			to_add.type = TOKEN_BLOCK_OPEN;
			break;

		case ('}'):
			to_add.type = TOKEN_BLOCK_CLOSE;
			break;

		case (':'):
			to_add.type = TOKEN_COLON;
			break;

		case ('\"'):
			to_add.type = TOKEN_STRING_LITERAL;

			// Move onto the beginning of the string
			INC_POSITION;

			// Loop until the end of the string
			while (CUR_CHAR != '\"')
			{
				to_add.length++;
				INC_POSITION;
			}

			// Compensate for the string close
			to_add.length++;

			break;

		case ('\''):
			to_add.type = TOKEN_STRING_LITERAL;

			// Move onto the beginning of the string
			INC_POSITION;

			// Loop until the end of the string
			while (CUR_CHAR != '\'')
			{
				to_add.length++;
				INC_POSITION;
			}

			// Compensate for the string close
			to_add.length++;

			break;
	}

	// Add the token!
	token_list.Add(to_add);

	// Move onto the next character
	INC_POSITION;

	return (1);

	outfunc;
}


int Tokeniser::AddToken(void)
{
	infunc(Tokeniser::AddToken);

	// Determine which type of token to add
	switch (FirstCharTable[CUR_CHAR])
	{
		case (CHARTYPE_ALPHA): return (AddAlphaToken());
		case (CHARTYPE_DIGIT): return (AddNumberToken());
		case (CHARTYPE_OPERATOR): return (AddOperatorToken());
		case (CHARTYPE_MISC): return (AddMiscToken());
	}

	// Shouldn't be possible to get here
	return (1);

	outfunc;
}


int Tokeniser::GenerateTokenPacket(void)
{
	// Packet size plus the extra token at the end
	int		count = TOKEN_PACKET_SIZE + 1;
	int		done = 0;

	infunc(Tokeniser::GenerateTokenPacket);

	// Loop until the entire packet is read or the end of the file is reached
	while (BYTES_LEFT && count)
	{
		// Increase the line count
		if (CUR_CHAR == 13)
			current_line++;

		// Inside a C++ style comment?
		if (in_cpp_comment)
		{
			// Look for the physical end of line to end the comments
			if (CUR_CHAR == 13)
				in_cpp_comment = 0;

			INC_POSITION;
			continue;
		}

		// Is this character to be ignored?
		if (FirstCharTable[CUR_CHAR] == CHARTYPE_IGNORE)
		{
			INC_POSITION;
			continue;
		}

		// Add it!
		AddToken();

		if (!in_cpp_comment)
		{
			// Counter how many tokens have passed
			count--;
			done++;
		}
	}

	return (done);

	outfunc;
}


void Tokeniser::IncToken(void)
{
	Token	null = { TOKEN_NULL, 0, 0 };

	infunc(Tokeniser::NextToken);

	// Move onto the next token
	token_pos++;

	// Reached the end of the current packet?
	if (token_pos == packet_size - 1)
	{
		// Reset the list
		token_list.Reset();

		// Shift the token that needs to be the previous one to the front
		token_list.Add(token_list(packet_size - 2));

		// Before adding new tokens, check to see if EOF has been reached
		if (!BYTES_LEFT)
		{
			token_list(0) = null;
			return;
		}

		// Generate the token packet (size plus the initial token)
		packet_size = GenerateTokenPacket() + 1;

		// Has the end of the file been hit?
		if (packet_size != TOKEN_PACKET_SIZE + 2)
		{
			// Shove a NULL token at the end
			token_list.Add(null);
			packet_size++;
		}
		else
		{
			// Cycle the position back to compensate for reading the extra token at the end
			position -= token_list(packet_size - 1).length;
		}

		// Move onto the first legal token
		token_pos = 1;
	}

	outfunc;
}


void Tokeniser::IsolateString(Token &token, char *store)
{
	infunc(Tokeniser::IsolateString);

	int		x;

	// Read the string from the data
	for (x = 0; x < token.length; x++)
		store[x] = data[token.position + x];

	// Terminate it
	store[x] = 0;

	outfunc;
}
