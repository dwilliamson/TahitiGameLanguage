
#ifndef	_INCLUDED_TOKEN_H
#define	_INCLUDED_TOKEN_H


#define IS_ALPHA(x)					\
	((x >= 'a' && x <= 'z') ||		\
	 (x >= 'A' && x <= 'Z') ||		\
	 (x == '_'))

#define IS_NUM(x)					\
	(x >= '0' && x <= '9')

#define DO_KEYWORD(keyword)						\
	if (!strcmp(string, #keyword))				\
	{											\
		Token *token = new Token;				\
		token->m_Type = TOKEN_##keyword;		\
		token->m_Location = pos;				\
		token->m_Length = len;					\
		token->m_Line = line;					\
		g_TokenList.Add(token);					\
		continue;								\
	}


enum
{
	TOKEN_vm_START_METHOD_REGISTER,
	TOKEN_vm_END_METHOD_REGISTER,
	TOKEN_vm_REGISTER_METHOD,
	TOKEN_vm_REGISTER_CLASS,
	TOKEN_NAME,
	TOKEN_STRING,
	TOKEN_COMMENT
};


class Token
{
public:
	// Type of token
	int		m_Type;

	// Byte location within the file
	int		m_Location;

	// Length of the token
	int		m_Length;

	// Line of the file the token is on
	int		m_Line;
};


#endif	/* _INCLUDED_TOKEN_H */