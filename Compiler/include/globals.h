
#ifndef	_INCLUDED_GLOBALS_H
#define	_INCLUDED_GLOBALS_H


#define	MAX_LABEL_SIZE		256


#define XOR(a, b)			(((a) && !(b)) || (!(a) && (b)))


extern	CFileSystem		g_file_system;
extern	CCallStack		g_call_stack;


typedef unsigned char	OpcodeType;


enum
{
	OPCODE_NOP = 0,
	OPCODE_PUSH_IMMEDIATE,
	OPCODE_FPUSH_IMMEDIATE,
	OPCODE_PUSH_MEMBER_VALUE,
	OPCODE_PUSH_MEMBER_ADDRESS,
	OPCODE_PUSH_LOCAL_VALUE,
	OPCODE_PUSH_LOCAL_ADDRESS,
	OPCODE_PUSH_RETURN,			/* Push the return register on the stack */
	OPCODE_POP_RETURN,			/* Pop a value off the stack and into the return register */
	OPCODE_POP,					/* Pop a value off the stack into nowhere */
	OPCODE_ADD,
	OPCODE_SUB,
	OPCODE_MUL,
	OPCODE_DIV,
	OPCODE_MOD,
	OPCODE_AND,
	OPCODE_OR,
	OPCODE_XOR,
	OPCODE_SHL,
	OPCODE_SHR,
	OPCODE_NOT,
	OPCODE_NEG,
	OPCODE_INC,
	OPCODE_DEC,
	OPCODE_SETE,
	OPCODE_SETNE,
	OPCODE_SETL,
	OPCODE_SETLE,
	OPCODE_SETG,
	OPCODE_SETGE,
	OPCODE_SETEZ,
	OPCODE_JMP,
	OPCODE_JZ,
	OPCODE_JNZ,
	OPCODE_JG,
	OPCODE_JGE,
	OPCODE_JL,
	OPCODE_JLE,
	OPCODE_JR,
	OPCODE_ASSIGN,			/* Set the value of address Stack[-1] to Stack[0] and pop both off */
	OPCODE_ASSIGN_PUSH,		/* Set the value of address Stack[-1] to Stack[0], pop both off and push the value back on */
	OPCODE_ASSIGN_REPLACE,	/* Set the value of Stack[0] to the value stored at the address Stack[0] */
	OPCODE_INSTF,			/* Push EBP, EBP = ESP */
	OPCODE_OUTSTF,			/* Pop EBP */
	OPCODE_ADDST,			/* Add a value to the stack pointer */
	OPCODE_SUBST,			/* Subtract a value from the stack pointer (maybe zero out the empty space, too) */
	OPCODE_RET,				/* Pop return_address, jump to it */
	OPCODE_RPV,				/* Push Stack[0] to Stack[-1], read the value at address Stack[-1] and store it in Stack[0] */
	OPCODE_DECR,			/* Decrement the value stored at the address in Stack[0] and replace Stack[0] with that value */
	OPCODE_INCR,			/* Increment the value stored at the address in Stack[0] and replace Stack[0] with that value */
	OPCODE_DECP,			/* Decrement the value stored at the address in Stack[0] and pop it off */
	OPCODE_INCP,			/* Increment the value stored at the address in Stack[0] and pop it off */
	OPCODE_DECM,			/* Replace the address at Stack[0] with the value at that address and then decrement the value at that address */
	OPCODE_INCM,			/* Replace the address at Stack[0] with the value at that address and then increment the value at that address */
	OPCODE_CALL,			/* Normal address call */
	OPCODE_CALLV,			/* Call virtual function */
	OPCODE_ARRAYP,			/* Move the address of the provided variable, plus 4, into the variable */
	OPCODE_ARRAYI,			/* Multiplies the index on the top of the stack by 4 and adds it to the address below it */
	OPCODE_FTOI,			/* Float to int */
	OPCODE_FTOS,			/* Float to short */
	OPCODE_FTOC,			/* Float to char */
	OPCODE_FTOUS,			/* Float to unsigned short */
	OPCODE_FTOUC,			/* Float to unsigned char */
	OPCODE_ITOF,			/* Int to float */
	OPCODE_ITOS,			/* Int to short */
	OPCODE_ITOC,			/* Int to char */
	OPCODE_ITOUS,			/* Int to unsigned short */
	OPCODE_ITOUC,			/* Int to unsigned char */
	OPCODE_FADD,
	OPCODE_FSUB,
	OPCODE_FMUL,
	OPCODE_FDIV,
	OPCODE_FMOD,
	OPCODE_FNEG,
	OPCODE_FSETE,
	OPCODE_FSETNE,
	OPCODE_FSETL,
	OPCODE_FSETLE,
	OPCODE_FSETG,
	OPCODE_FSETGE,
	OPCODE_FSETEZ,
	OPCODE_FJZ,
	OPCODE_FJNZ,
	OPCODE_FJG,
	OPCODE_FJGE,
	OPCODE_FJL,
	OPCODE_FJLE,
	OPCODE_FDECR,
	OPCODE_FINCR,
	OPCODE_FDECP,
	OPCODE_FINCP,
	OPCODE_FDECM,
	OPCODE_FINCM,
	OPCODE_PUSH_MEMBER_ARRAY,	/* Push the value on the stack and add the current data segment to it */
	OPCODE_PUSH_LOCAL_ARRAY,
	OPCODE_SETSTATE,
	OPCODE_ENDSTATE,
	OPCODE_CALLIM,

	MAX_NB_OPCODES
};


#endif