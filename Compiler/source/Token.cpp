
#include "..\include\main.h"


TokenInfo	tkinfo_list[] =
{
	{ TOKEN_GROUP_NONE,		0,		OPCODE_NOP		},
	{ TOKEN_GROUP_NAME,		16,		OPCODE_NOP		},
	{ TOKEN_GROUP_NUMBER,	16,		OPCODE_NOP		},
	{ TOKEN_GROUP_OPERATOR, 11,		OPCODE_ADD		},
	{ TOKEN_GROUP_OPERATOR, 1,		OPCODE_NOP		},
	{ TOKEN_GROUP_OPERATOR, 14,		OPCODE_NOP		},
	{ TOKEN_GROUP_OPERATOR, 11,		OPCODE_SUB		},
	{ TOKEN_GROUP_OPERATOR, 1,		OPCODE_NOP		},
	{ TOKEN_GROUP_OPERATOR, 14,		OPCODE_NOP		},
	{ TOKEN_GROUP_OPERATOR, 12,		OPCODE_DIV		},
	{ TOKEN_GROUP_OPERATOR, 1,		OPCODE_NOP		},
	{ TOKEN_GROUP_OPERATOR, 12,		OPCODE_MUL		},
	{ TOKEN_GROUP_OPERATOR, 1,		OPCODE_NOP		},
	{ TOKEN_GROUP_OPERATOR, 7,		OPCODE_AND		},
	{ TOKEN_GROUP_OPERATOR, 1,		OPCODE_NOP		},
	{ TOKEN_GROUP_OPERATOR, 4,		OPCODE_NOP		},
	{ TOKEN_GROUP_OPERATOR, 6,		OPCODE_XOR		},
	{ TOKEN_GROUP_OPERATOR, 1,		OPCODE_NOP		},
	{ TOKEN_GROUP_OPERATOR, 5,		OPCODE_OR		},
	{ TOKEN_GROUP_OPERATOR, 1,		OPCODE_NOP		},
	{ TOKEN_GROUP_OPERATOR, 3,		OPCODE_NOP		},
	{ TOKEN_GROUP_OPERATOR, 1,		OPCODE_NOP		},
	{ TOKEN_GROUP_OPERATOR, 1,		OPCODE_NOP		},
	{ TOKEN_GROUP_OPERATOR, 1,		OPCODE_NOP		},
	{ TOKEN_GROUP_OPERATOR, 8,		OPCODE_SETE		},
	{ TOKEN_GROUP_OPERATOR, 0,		OPCODE_NOP		},
	{ TOKEN_GROUP_OPERATOR, 0,		OPCODE_NOP		},
	{ TOKEN_GROUP_OPERATOR, 12,		OPCODE_MOD		},
	{ TOKEN_GROUP_OPERATOR, 1,		OPCODE_NOP		},
	{ TOKEN_GROUP_OPERATOR, 14,		OPCODE_NOP		},
	{ TOKEN_GROUP_OPERATOR, 8,		OPCODE_SETNE	},
	{ TOKEN_GROUP_OPERATOR, 9,		OPCODE_SETL		},
	{ TOKEN_GROUP_OPERATOR, 9,		OPCODE_SETLE	},
	{ TOKEN_GROUP_OPERATOR, 10,		OPCODE_SHL		},
	{ TOKEN_GROUP_OPERATOR, 9,		OPCODE_SETG		},
	{ TOKEN_GROUP_OPERATOR, 9,		OPCODE_SETGE	},
	{ TOKEN_GROUP_OPERATOR, 10,		OPCODE_SHR		},
	{ TOKEN_GROUP_OPERATOR, 14,		OPCODE_NOP		},
	{ TOKEN_GROUP_OPERATOR, 0,		OPCODE_NOP		},
	{ TOKEN_GROUP_MISC,		0,		OPCODE_NOP		},
	{ TOKEN_GROUP_MISC,		0,		OPCODE_NOP		},
	{ TOKEN_GROUP_MISC,		0,		OPCODE_NOP		},
	{ TOKEN_GROUP_MISC,		0,		OPCODE_NOP		},
	{ TOKEN_GROUP_MISC,		0,		OPCODE_NOP		},
	{ TOKEN_GROUP_OPERATOR,	15,		OPCODE_NOP		},
	{ TOKEN_GROUP_OPERATOR,	0,		OPCODE_NOP		},
	{ TOKEN_GROUP_KEYWORD,	0,		OPCODE_NOP		},
	{ TOKEN_GROUP_KEYWORD,	0,		OPCODE_NOP		},
	{ TOKEN_GROUP_KEYWORD,	0,		OPCODE_NOP		},
	{ TOKEN_GROUP_KEYWORD,	0,		OPCODE_NOP		},
	{ TOKEN_GROUP_KEYWORD,	0,		OPCODE_NOP		},
	{ TOKEN_GROUP_KEYWORD,	0,		OPCODE_NOP		},
	{ TOKEN_GROUP_KEYWORD,	0,		OPCODE_NOP		},
	{ TOKEN_GROUP_KEYWORD,	0,		OPCODE_NOP		},
	{ TOKEN_GROUP_KEYWORD,	0,		OPCODE_NOP		},
	{ TOKEN_GROUP_KEYWORD,	0,		OPCODE_NOP		},
	{ TOKEN_GROUP_KEYWORD,	0,		OPCODE_NOP		},
	{ TOKEN_GROUP_KEYWORD,	0,		OPCODE_NOP		},
	{ TOKEN_GROUP_KEYWORD,	0,		OPCODE_NOP		},
	{ TOKEN_GROUP_KEYWORD,	0,		OPCODE_NOP		},
	{ TOKEN_GROUP_KEYWORD,	0,		OPCODE_NOP		},
	{ TOKEN_GROUP_KEYWORD,	0,		OPCODE_NOP		},
	{ TOKEN_GROUP_KEYWORD,	0,		OPCODE_NOP		},
	{ TOKEN_GROUP_KEYWORD,	0,		OPCODE_NOP		},
	{ TOKEN_GROUP_KEYWORD,	0,		OPCODE_NOP		},
	{ TOKEN_GROUP_KEYWORD,	0,		OPCODE_NOP		},
	{ TOKEN_GROUP_KEYWORD,	0,		OPCODE_NOP		},
	{ TOKEN_GROUP_KEYWORD,	0,		OPCODE_NOP		},
	{ TOKEN_GROUP_KEYWORD,	0,		OPCODE_NOP		},
	{ TOKEN_GROUP_KEYWORD,	0,		OPCODE_NOP		},
	{ TOKEN_GROUP_KEYWORD,	0,		OPCODE_NOP		},
	{ TOKEN_GROUP_KEYWORD,	0,		OPCODE_NOP		},
	{ TOKEN_GROUP_KEYWORD,	0,		OPCODE_NOP		},
	{ TOKEN_GROUP_KEYWORD,	0,		OPCODE_NOP		},
	{ TOKEN_GROUP_KEYWORD,	0,		OPCODE_NOP		},
	{ TOKEN_GROUP_KEYWORD,	0,		OPCODE_NOP		},
	{ TOKEN_GROUP_MISC,		0,		OPCODE_NOP		},
	{ TOKEN_GROUP_KEYWORD,	0,		OPCODE_NOP		},
	{ TOKEN_GROUP_KEYWORD,	0,		OPCODE_NOP		},
	{ TOKEN_GROUP_KEYWORD,	0,		OPCODE_NOP		}
};


void Token::PrintInfo(VMCFile &parent)
{
	int		x;
	char	string[MAX_LABEL_SIZE];

	infunc(Token::PrintInfo);

	// Title
	printf("Token: ");

	#define IS_TYPE(x)	case (x): printf("%25s ", #x); break;

	// Token type
	switch (type)
	{
		IS_TYPE(TOKEN_NULL);
		IS_TYPE(TOKEN_NAME);
		IS_TYPE(TOKEN_NUMBER);
		IS_TYPE(TOKEN_PLUS);
		IS_TYPE(TOKEN_PLUS_ASSIGN);
		IS_TYPE(TOKEN_PLUS_PLUS);
		IS_TYPE(TOKEN_MINUS);
		IS_TYPE(TOKEN_MINUS_ASSIGN);
		IS_TYPE(TOKEN_MINUS_MINUS);
		IS_TYPE(TOKEN_DIVIDE);
		IS_TYPE(TOKEN_DIVIDE_ASSIGN);
		IS_TYPE(TOKEN_MULTIPLY);
		IS_TYPE(TOKEN_MULTIPLY_ASSIGN);
		IS_TYPE(TOKEN_BITWISE_AND);
		IS_TYPE(TOKEN_BITWISE_AND_ASSIGN);
		IS_TYPE(TOKEN_LOGICAL_AND);
		IS_TYPE(TOKEN_BITWISE_XOR);
		IS_TYPE(TOKEN_BITWISE_XOR_ASSIGN);
		IS_TYPE(TOKEN_BITWISE_OR);
		IS_TYPE(TOKEN_BITWISE_OR_ASSIGN);
		IS_TYPE(TOKEN_LOGICAL_OR);
		IS_TYPE(TOKEN_ASSIGN);
		IS_TYPE(TOKEN_EQUAL);
		IS_TYPE(TOKEN_LEFT_BRACKET);
		IS_TYPE(TOKEN_RIGHT_BRACKET);
		IS_TYPE(TOKEN_MODULUS);
		IS_TYPE(TOKEN_MODULUS_ASSIGN);
		IS_TYPE(TOKEN_LOGICAL_NOT);
		IS_TYPE(TOKEN_LOGICAL_NOT_EQUAL);
		IS_TYPE(TOKEN_LESS);
		IS_TYPE(TOKEN_LESS_EQUAL);
		IS_TYPE(TOKEN_SHIFT_LEFT);
		IS_TYPE(TOKEN_GREATER);
		IS_TYPE(TOKEN_GREATER_EQUAL);
		IS_TYPE(TOKEN_SHIFT_RIGHT);
		IS_TYPE(TOKEN_BITWISE_NOT);
		IS_TYPE(TOKEN_COMMA);
		IS_TYPE(TOKEN_END_OF_LINE);
		IS_TYPE(TOKEN_BLOCK_OPEN);
		IS_TYPE(TOKEN_BLOCK_CLOSE);
		default: printf("%20s", "Unknown");
	}

	#undef	IS_TYPE

	// Position and length
	printf("%5d %5d", position, length);

	// String in the file
	for (x = 0; x < length; x++)
		string[x] = parent.data[position + x];

	string[x] = 0;

	printf("%20s\n", string);

	outfunc;
}


int Token::Group(void)
{
	infunc(Token::Group);
	return (tkinfo_list[type].group);
	outfunc;
}


int Token::Priority(void)
{
	infunc(Token::Priority);
	return (tkinfo_list[type].priority);
	outfunc;
}


OpcodeType Token::Opcode(void)
{
	infunc(Token::Opcode);
	return (tkinfo_list[type].opcode);
	outfunc;
}


int Token::Is(int values)
{
	infunc(Token::Is);

	int		is = 1;

	// Check for an assignment type
	if (values & TOKEN_TYPE_ASSIGNMENT)
	{
		switch (type)
		{
			// It is of that type
			case (TOKEN_ASSIGN):
			case (TOKEN_PLUS_ASSIGN):
			case (TOKEN_MINUS_ASSIGN):
			case (TOKEN_DIVIDE_ASSIGN):
			case (TOKEN_MULTIPLY_ASSIGN):
			case (TOKEN_BITWISE_AND_ASSIGN):
			case (TOKEN_BITWISE_XOR_ASSIGN):
			case (TOKEN_BITWISE_OR_ASSIGN):
			case (TOKEN_SHL_ASSIGN):
			case (TOKEN_SHR_ASSIGN):
			case (TOKEN_MODULUS_ASSIGN):
				is = 1;
				break;

			// It isn't of that type
			default:
				is = 0;
				break;
		}

		// If this is a "is not a" case then flip the decision
		if (values & (TOKEN_TYPE_ASSIGNMENT << TTSHIFT))
			is = !is;

		// Everything must be true for success
		if (!is)
			return (0);
	}

	// Check for an operator type
	if (values & TOKEN_TYPE_OPERATOR)
	{
		switch (type)
		{
			// It is of that type
			case (TOKEN_PLUS):
			case (TOKEN_PLUS_ASSIGN):
			case (TOKEN_PLUS_PLUS):
			case (TOKEN_MINUS):
			case (TOKEN_MINUS_ASSIGN):
			case (TOKEN_MINUS_MINUS):
			case (TOKEN_DIVIDE):
			case (TOKEN_DIVIDE_ASSIGN):
			case (TOKEN_MULTIPLY):
			case (TOKEN_MULTIPLY_ASSIGN):
			case (TOKEN_BITWISE_AND):
			case (TOKEN_BITWISE_AND_ASSIGN):
			case (TOKEN_LOGICAL_AND):
			case (TOKEN_BITWISE_XOR):
			case (TOKEN_BITWISE_XOR_ASSIGN):
			case (TOKEN_BITWISE_OR):
			case (TOKEN_BITWISE_OR_ASSIGN):
			case (TOKEN_LOGICAL_OR):
			case (TOKEN_ASSIGN):
			case (TOKEN_SHL_ASSIGN):
			case (TOKEN_SHR_ASSIGN):
			case (TOKEN_EQUAL):
			case (TOKEN_MODULUS):
			case (TOKEN_MODULUS_ASSIGN):
			case (TOKEN_LOGICAL_NOT):
			case (TOKEN_LOGICAL_NOT_EQUAL):
			case (TOKEN_LESS):
			case (TOKEN_LESS_EQUAL):
			case (TOKEN_SHIFT_LEFT):
			case (TOKEN_GREATER):
			case (TOKEN_GREATER_EQUAL):
			case (TOKEN_SHIFT_RIGHT):
			case (TOKEN_BITWISE_NOT):
			// -----------------------
			case (TOKEN_LEFT_BRACKET):
			case (TOKEN_RIGHT_BRACKET):
			case (TOKEN_ARRAY_OPEN):
			case (TOKEN_ARRAY_CLOSE):
				is = 1;
				break;

			// It isn't of that type
			default:
				is = 0;
				break;
		}

		// If this is a "is not a" case then flip the decision
		if (values & (TOKEN_TYPE_OPERATOR << TTSHIFT))
			is = !is;

		// Everything must be true for success
		if (!is)
			return (0);
	}

	// Check for relational operators
	if (values & TOKEN_TYPE_RELOPS)
	{
		switch (type)
		{
			// Is of that type
			case (TOKEN_EQUAL):
			case (TOKEN_LOGICAL_NOT_EQUAL):
			case (TOKEN_LESS):
			case (TOKEN_LESS_EQUAL):
			case (TOKEN_GREATER):
			case (TOKEN_GREATER_EQUAL):
				is = 1;
				break;

			// It isn't of that type
			default:
				is = 0;
				break;
		}

		// If this is a "is not a" case then flip the decision
		if (values & (TOKEN_TYPE_BRACKETOPS << TTSHIFT))
			is = !is;

		// Everything must be true for success
		if (!is)
			return (0);
	}

	// Check for bracket-type operators (),[]
	if (values & TOKEN_TYPE_BRACKETOPS)
	{
		switch (type)
		{
			// Is of that type
			case (TOKEN_LEFT_BRACKET):
			case (TOKEN_RIGHT_BRACKET):
			case (TOKEN_ARRAY_OPEN):
			case (TOKEN_ARRAY_CLOSE):
				is = 1;
				break;

			// It isn't of that type
			default:
				is = 0;
				break;
		}

		// If this is a "is not a" case then flip the decision
		if (values & (TOKEN_TYPE_BRACKETOPS << TTSHIFT))
			is = !is;

		// Everything must be true for success
		if (!is)
			return (0);
	}

	// Check for the operators that force branching &&,||
	if (values & TOKEN_TYPE_BRANCHOPS)
	{
		switch (type)
		{
			// Is of that type
			case (TOKEN_LOGICAL_AND):
			case (TOKEN_LOGICAL_OR):
				is = 1;
				break;

			// It isn't of that type
			default:
				is = 0;
				break;
		}

		// If this is a "is not a" case then flip the decision
		if (values & (TOKEN_TYPE_BRANCHOPS << TTSHIFT))
			is = !is;

		// Everything must be true for success
		if (!is)
			return (0);
	}

	// Success!
	return (1);

	outfunc;
}