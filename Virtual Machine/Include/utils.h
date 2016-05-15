
#ifndef	_INCLUDED_UTILS_H
#define	_INCLUDED_UTILS_H


#define STACK_POP(type, th)														\
	*(type *)(&(((uBYTE *)th->m_Stack)[th->m_regSP += 4]))


#define STACK_PUSH(v, type, th)													\
	*(type *)(&(((uBYTE *)th->m_Stack)[th->m_regSP])) = v;						\
	th->m_regSP -= 4;


#define READ32(v, type, th)														\
	v = *(type *)(&(((uBYTE *)th->m_CodeSegment)[th->m_regIP]));				\
	th->m_regIP += 4;


#define MATH_OP(type, x, th)													\
	{																			\
		type	a, b;															\
																				\
		a = STACK_POP(type, th);												\
		b = STACK_POP(type, th);												\
		STACK_PUSH(a x b, type, th);											\
	}


#define MATH_OP_INV(type, x, th)												\
	{																			\
		type	a, b;															\
																				\
		a = STACK_POP(type, th);												\
		b = STACK_POP(type, th);												\
		STACK_PUSH(b x a, type, th);											\
	}


#define JUMP_OP_INV(type, x, th)												\
	{																			\
		type	a, b;															\
		int		address;														\
																				\
		a = STACK_POP(type, th);												\
		b = STACK_POP(type, th);												\
																				\
		if (b x a)																\
		{																		\
			READ32(address, int, th);											\
			th->m_regIP = address;												\
		}																		\
		else																	\
		{																		\
			th->m_regIP += 4;													\
		}																		\
	}


#define CONVERT_OP(type_a, type_b, sub_type, th)								\
	{																			\
		type_a	a;																\
		type_b	b;																\
																				\
		a = STACK_POP(type_a, th);												\
		b = (sub_type)a;														\
		STACK_PUSH(b, type_b, th);												\
	}


#endif	/* _INCLUDED_UTILS_H */